package main

// based off of https://github.com/minikomi/pipesock

import (
	"bufio"
	"code.google.com/p/go.net/websocket"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"github.com/gregose/go-serial/serial"
	"io"
	"log"
	"net/http"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

type Hub struct {
	Connections map[*Socket]bool
	Pipe        chan string
}

func ParseMessage(inStr string) ([]byte, error) {
	parts := strings.Split(strings.TrimSpace(inStr), "|")
	t := time.Now()
	ts := t.UnixNano() / 1e6
	switch parts[0] {
	case "T":
		if len(parts) != 4 {
			break
		}
		temp := new(TemperatureMessage)
		temp.MessageHeader.Type = "Temp"
		temp.MessageHeader.Timestamp = ts
		temp.Ambient, _ = strconv.ParseFloat(parts[1], 64)
		temp.Boiler, _ = strconv.ParseFloat(parts[2], 64)
		temp.GroupHead, _ = strconv.ParseFloat(parts[3], 64)
		return json.Marshal(temp)
	case "K":
		if len(parts) != 2 {
			break
		}
		knob := new(KnobMessage)
		knob.MessageHeader.Type = "Knob"
		knob.MessageHeader.Timestamp = ts
		knob.Value, _ = strconv.ParseFloat(parts[1], 64)
		return json.Marshal(knob)
	case "S":
		if len(parts) != 5 {
			break
		}
		switchState := new(SwitchStateMessage)
		switchState.MessageHeader.Type = "SwitchState"
		switchState.MessageHeader.Timestamp = ts
		switchState.Power = (parts[1] == "1")
		switchState.Brew = (parts[2] == "1")
		switchState.HotWater = (parts[3] == "1")
		switchState.Steam = (parts[4] == "1")
		return json.Marshal(switchState)
	case "P":
		if len(parts) != 11 {
			break
		}
		pid := new(PIDMessage)
		pid.MessageHeader.Type = "PID"
		pid.MessageHeader.Timestamp = ts
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

type Message struct {
	Timestamp int64
	Message   string
}

type MessageHeader struct {
	Timestamp int64
	Type      string
}

type TemperatureMessage struct {
	MessageHeader
	Ambient   float64
	Boiler    float64
	GroupHead float64
}

type KnobMessage struct {
	MessageHeader
	Value float64
}

type SwitchStateMessage struct {
	MessageHeader
	Power    bool
	Brew     bool
	HotWater bool
	Steam    bool
}

type PIDMessage struct {
	MessageHeader
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

type Broadcast struct {
	Timestamp int64
	Messages  []*Message
}

func (h *Hub) BroadcastLoop() {
	for {
		str := <-h.Pipe

		broadcastJSON, err := ParseMessage(str)

		if err != nil {
			log.Println("Buffer JSON Error: ", err)
			continue
		}

		log.Println(string(broadcastJSON))

		for s, _ := range h.Connections {
			err := websocket.Message.Send(s.Ws, string(broadcastJSON))
			if err != nil {
				if err.Error() != ("use of closed network connection") {
					log.Println("WS error:", err)
				}
				s.Ws.Close()
				delete(h.Connections, s)
			}
		}

		if len(messageBuffer) > bufferSize {
			messageBuffer = messageBuffer[1:]
		}
		messageBuffer = append(messageBuffer, string(broadcastJSON))
	}
}

type Socket struct {
	Ws *websocket.Conn
}

func (s *Socket) ReceiveMessage() {

	for {
		var x []byte
		err := websocket.Message.Receive(s.Ws, &x)
		if err != nil {
			break
		}
	}
	s.Ws.Close()
}

func serialLoop() {
	c := serial.OpenOptions {
		PortName: serialPort,
		BaudRate: baudRate,
		DataBits: 8,
		StopBits: 1,
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
				fmt.Print(str)
			}

			h.Pipe <- str
		}
	}
}

func IndexHandler(w http.ResponseWriter, req *http.Request) {
	var filePath string

	if req.URL.Path == "/" {
		filePath = fmt.Sprintf("%s/index.html", viewPath)
	} else {
		filePath = fmt.Sprintf("%s/%s", viewPath, req.URL.Path)
	}
	if logging {
		log.Println(filePath)
	}
	http.ServeFile(w, req, filePath)
}

func BufferHandler(w http.ResponseWriter, req *http.Request) {
	bufferJSON, err := json.Marshal(messageBuffer)
	if err != nil {
		log.Println("Buffer JSON error:", err)
		return
	}
	fmt.Fprintf(w, string(bufferJSON))
}

func FlushHandler(w http.ResponseWriter, req *http.Request) {
	messageBuffer = messageBuffer[:0]
	fmt.Fprintf(w, "Flushed")
}

func wsServer(ws *websocket.Conn) {
	s := &Socket{ws}
	h.Connections[s] = true
	s.ReceiveMessage()
}

var (
	h                    Hub
	viewPath             string
	port, bufferSize     int
	passThrough, logging bool
	messageBuffer        []string
	serialPort           string
	baudRate             uint
)

func init() {
	flag.IntVar(&port, "port", 9193, "Port for the pipesock to sit on.")
	flag.IntVar(&port, "p", 9193, "Port for the pipesock to sit on (shorthand).")

	flag.BoolVar(&passThrough, "through", false, "Pass serial input to STDOUT.")
	flag.BoolVar(&passThrough, "t", false, "Pass serial input to STDOUT (shorthand).")

	flag.BoolVar(&logging, "log", false, "Log HTTP requests to STDOUT")
	flag.BoolVar(&logging, "l", false, "Log HTTP requests t STDOUT (shorthand).")

	flag.StringVar(&viewPath, "view", "default", "View directory to serve.")
	flag.StringVar(&viewPath, "v", "default", "View directory to serve (shorthand).")

	flag.IntVar(&bufferSize, "num", 60*15, "Number of previous broadcasts to keep in memory.")
	flag.IntVar(&bufferSize, "n", 60*15, "Number of previous broadcasts to keep in memory (shorthand).")

	flag.UintVar(&baudRate, "baud", 57600, "Serial port baud rate.")
	flag.UintVar(&baudRate, "b", 57600, "Serial port baud rate.")

	flag.StringVar(&serialPort, "serial", "/dev/tty.usbmodem1411", "Serial port device.")
	flag.StringVar(&serialPort, "s", "/dev/tty.usbmodem1411", "Serial port device.")

	messageBuffer = make([]string, 0)

	// Set up hub
	h.Connections = make(map[*Socket]bool)
	h.Pipe = make(chan string, 1)
}

func main() {
	flag.Parse()

	viewPath = filepath.Join(".", "views", viewPath)

	go h.BroadcastLoop()
	go serialLoop()

	http.HandleFunc("/", IndexHandler)
	http.HandleFunc("/buffer.json", BufferHandler)
	http.HandleFunc("/flush", FlushHandler)
	http.Handle("/ws", websocket.Handler(wsServer))

	portString := fmt.Sprintf(":%d", port)
	err := http.ListenAndServe(portString, nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
