package com.softwinner.dragonbox.platform;

public class GpioPlatform implements IGpioPlatform {
	public void writeGpio(int status) {
		char portType = 'h';
		int portNum = 20;
		com.softwinner.Gpio.writeGpio(portType, portNum, status);
	}
}
