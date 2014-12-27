package main

// based off of https://github.com/minikomi/pipesock

import (
	"bufio"
	"code.google.com/p/go.net/websocket"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"github.com/elazarl/go-bindata-assetfs"
	"github.com/gregose/go-serial/serial"
	"io"
	"log"
	"math/rand"
	"net/http"
	"strconv"
	"strings"
	"time"
)

type socket struct {
	Ws *websocket.Conn
}

type hub struct {
	Connections map[*socket]bool
	Pipe        chan string
}

type messageHeader struct {
	Timestamp int64
	Type      string
}

type temperatureMessage struct {
	Header    messageHeader
	Ambient   float64
	Boiler    float64
	Grouphead float64
}

type knobMessage struct {
	Header messageHeader
	Value  float64
}

type switchStateMessage struct {
	Header   messageHeader
	Power    bool
	Brew     bool
	HotWater bool
	Steam    bool
}

type pidMessage struct {
	Header     messageHeader
	dT         float64
	SetPoint   float64
	Input      float64
	Output     float64
	Error      float64
	Integral   float64
	Derivative float64
	kP         float64
	kI         float64
	kD         float64
}

type temperatureLogValue struct {
	Time  int64   `json:"time"`
	Value float64 `json:"y"`
}

type temperatureLogEntry struct {
	Label  string                `json:"label"`
	Values []temperatureLogValue `json:"values"`
}

type temperatureLog struct {
	LogSize    int
	MaxLogSize int
	Data       []temperatureLogEntry
}

func parseMessage(inStr string) ([]byte, error) {
	parts := strings.Split(strings.TrimSpace(inStr), "|")
	t := time.Now()
	ts := t.UnixNano() / 1e6
	switch parts[0] {
	case "T":
		if len(parts) != 4 {
			break
		}
		temp := new(temperatureMessage)
		temp.Header.Type = "Temp"
		temp.Header.Timestamp = ts
		temp.Ambient, _ = strconv.ParseFloat(parts[1], 64)
		temp.Boiler, _ = strconv.ParseFloat(parts[2], 64)
		temp.Grouphead, _ = strconv.ParseFloat(parts[3], 64)

		// Boiler log buffer
		if len(tempLog.Data[0].Values) >= maxLogSize {
			tempLog.Data[0].Values = tempLog.Data[0].Values[1:]
		}
		boilerValue := temperatureLogValue{Time: ts / 1000, Value: temp.Boiler}
		tempLog.Data[0].Values = append(tempLog.Data[0].Values, boilerValue)

		// Grouphead log buffer
		if len(tempLog.Data[1].Values) >= maxLogSize {
			tempLog.Data[1].Values = tempLog.Data[1].Values[1:]
		}
		groupheadValue := temperatureLogValue{Time: ts / 1000, Value: temp.Grouphead}
		tempLog.Data[1].Values = append(tempLog.Data[1].Values, groupheadValue)
		tempLog.LogSize = len(tempLog.Data[0].Values)

		return json.Marshal(temp)
	case "K":
		if len(parts) != 2 {
			break
		}
		knob := new(knobMessage)
		knob.Header.Type = "Knob"
		knob.Header.Timestamp = ts
		knob.Value, _ = strconv.ParseFloat(parts[1], 64)
		return json.Marshal(knob)
	case "S":
		if len(parts) != 5 {
			break
		}
		switchState := new(switchStateMessage)
		switchState.Header.Type = "SwitchState"
		switchState.Header.Timestamp = ts
		switchState.Power = (parts[1] == "1")
		switchState.Brew = (parts[2] == "1")
		switchState.HotWater = (parts[3] == "1")
		switchState.Steam = (parts[4] == "1")
		return json.Marshal(switchState)
	case "P":
		if len(parts) != 11 {
			break
		}
		pid := new(pidMessage)
		pid.Header.Type = "PID"
		pid.Header.Timestamp = ts
		pid.dT, _ = strconv.ParseFloat(parts[1], 64)
		pid.SetPoint, _ = strconv.ParseFloat(parts[2], 64)
		pid.Input, _ = strconv.ParseFloat(parts[3], 64)
		pid.Output, _ = strconv.ParseFloat(parts[4], 64)
		pid.Error, _ = strconv.ParseFloat(parts[5], 64)
		pid.Integral, _ = strconv.ParseFloat(parts[6], 64)
		pid.Derivative, _ = strconv.ParseFloat(parts[7], 64)
		pid.kP, _ = strconv.ParseFloat(parts[8], 64)
		pid.kI, _ = strconv.ParseFloat(parts[9], 64)
		pid.kD, _ = strconv.ParseFloat(parts[10], 64)
		return json.Marshal(pid)
	}
	return nil, errors.New("message type not found")
}

func (h *hub) broadcastLoop() {
	for {
		str := <-h.Pipe

		broadcastJSON, err := parseMessage(str)

		if err != nil {
			log.Println("Buffer JSON Error: ", err)
			continue
		}

		log.Println(string(broadcastJSON))

		for s := range h.Connections {
			err := websocket.Message.Send(s.Ws, string(broadcastJSON))
			if err != nil {
				if err.Error() != ("use of closed network connection") {
					log.Println("WS error:", err)
				}
				s.Ws.Close()
				delete(h.Connections, s)
			}
		}
	}
}

func (s *socket) receiveMessage() {
	for {
		var x []byte
		err := websocket.Message.Receive(s.Ws, &x)
		if err != nil {
			break
		}
	}
	s.Ws.Close()
}

// Generate random temp values for dev mode
func devLoop() {
	for {
		if rand.Intn(2) < 1 {
			lastDevGrouphead += (float64(rand.Intn(200)) / 10.0)
		} else {
			lastDevGrouphead -= (float64(rand.Intn(200)) / 10.0)
		}

		if rand.Intn(2) < 1 {
			lastDevBoiler += (float64(rand.Intn(200)) / 10.0)
		} else {
			lastDevBoiler -= (float64(rand.Intn(200)) / 10.0)
		}

		str := fmt.Sprintf("T|80.1|%f|%f", lastDevBoiler, lastDevGrouphead)
		h.Pipe <- str
		time.Sleep(time.Duration(1) * time.Second)
	}
}

func serialLoop() {
	c := serial.OpenOptions{
		PortName:        serialPort,
		BaudRate:        baudRate,
		DataBits:        8,
		StopBits:        1,
		MinimumReadSize: 4,
	}

	for {
		s, err := serial.Open(c)

		if err != nil {
			log.Println(err)
			time.Sleep(time.Duration(1) * time.Second)
			continue
		}

		r := bufio.NewReader(s)
		for {
			str, err := r.ReadString('\n')

			if err == io.EOF {
				log.Println("EOF. Server closed.")
				break
			}
			if err != nil {
				log.Println("Read Line Error:", err)
				continue
			}
			if len(str) == 0 {
				continue
			}

			if passThrough {
				log.Print(str)
			}

			h.Pipe <- str
		}
	}
}

func bufferHandler(w http.ResponseWriter, req *http.Request) {
	bufferJSON, err := json.Marshal(tempLog)
	if err != nil {
		log.Println("Buffer JSON error:", err)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	w.Write(bufferJSON)
}

func flushHandler(w http.ResponseWriter, req *http.Request) {
	tempLog.Data[0].Values = tempLog.Data[0].Values[:0]
	tempLog.Data[1].Values = tempLog.Data[1].Values[:0]
	tempLog.LogSize = 0
	w.Write([]byte("flushed"))
}

func eventServer(ws *websocket.Conn) {
	s := &socket{ws}
	h.Connections[s] = true
	s.receiveMessage()
}

var (
	h                             hub
	viewPath                      string
	port, maxLogSize              int
	passThrough, logging, devMode bool
	tempLog                       temperatureLog
	serialPort                    string
	baudRate                      uint
	lastDevGrouphead              float64
	lastDevBoiler                 float64
)

func init() {
	flag.IntVar(&port, "port", 8080, "Webserver port.")
	flag.IntVar(&port, "p", 8080, "Webserver port (shorthand).")

	flag.BoolVar(&passThrough, "through", false, "Pass serial input to STDOUT.")
	flag.BoolVar(&passThrough, "t", false, "Pass serial input to STDOUT (shorthand).")

	flag.BoolVar(&logging, "log", false, "Log HTTP requests to STDOUT")
	flag.BoolVar(&logging, "l", false, "Log HTTP requests to STDOUT (shorthand).")

	flag.BoolVar(&devMode, "dev", false, "Dev mode")
	flag.BoolVar(&devMode, "d", false, "Dev mode (shorthand).")

	flag.IntVar(&maxLogSize, "num", 180, "Number of previous temperatures to keep in memory.")
	flag.IntVar(&maxLogSize, "n", 180, "Number of previous temperatures to keep in memory (shorthand).")

	flag.UintVar(&baudRate, "baud", 57600, "Serial port baud rate.")
	flag.UintVar(&baudRate, "b", 57600, "Serial port baud rate.")

	flag.StringVar(&serialPort, "serial", "/dev/ttyACM0", "Serial port device.")
	flag.StringVar(&serialPort, "s", "/dev/ttyACM0", "Serial port device.")

	// Dev starting values
	lastDevGrouphead = 100.0
	lastDevBoiler = 180.0

	// Set up hub
	h.Connections = make(map[*socket]bool)
	h.Pipe = make(chan string, 1)
}

func main() {
	flag.Parse()

	tempLog = temperatureLog{LogSize: 0, MaxLogSize: maxLogSize, Data: make([]temperatureLogEntry, 2)}
	tempLog.Data[0] = temperatureLogEntry{Label: "Boiler", Values: make([]temperatureLogValue, 0)}
	tempLog.Data[1] = temperatureLogEntry{Label: "Grouphead", Values: make([]temperatureLogValue, 0)}

	go h.broadcastLoop()

	if devMode {
		go devLoop()
	} else {
		go serialLoop()
	}

	http.HandleFunc("/buffer.json", bufferHandler)
	http.HandleFunc("/flush", flushHandler)
	http.Handle("/events", websocket.Handler(eventServer))
	http.Handle("/",
		http.FileServer(
			&assetfs.AssetFS{Asset: Asset, AssetDir: AssetDir, Prefix: "views"}))

	portString := fmt.Sprintf(":%d", port)
	err := http.ListenAndServe(portString, nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
