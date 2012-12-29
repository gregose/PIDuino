#simplest ruby program to read from arduino serial, 
#using the SerialPort gem
#(http://rubygems.org/gems/serialport)

require "serialport"

#params for serial port
port_str = "COM3"  #may be different for you
baud_rate = 57600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

#just read forever
File.open('log.dat', 'wb') do |f|
  	f.sync = true
  	f.write("Time|T|Ambient|Boiler|Brewgroup\n")
	while true do
		begin
			c = sp.readline
	  		if(!c.nil?)
	  			l = "#{Time.now.to_f}|#{c}"
				puts l
				if(c.match(/^T\|/))
					f.write(l)
				end
    		end
    	rescue
    		puts "no data"
    	end 
	end
end

sp.close                       #see note 1