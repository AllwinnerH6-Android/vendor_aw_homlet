package com.softwinner.dragonbox.entity;

public class NetReceivedResult {
	public String sResult=null;//网络操作结果
	public String sReason=null;//网络操作结果原因
	public String sCommand=null;//操作命令
	public int iCode = 0;//服务器返回http命令
	@Override
	public String toString() {
		return "NetReceivedResult: \nCommand: "+sCommand+
				"\nResult: "+sResult+"\nReason: "+sReason+
				"\nCode: "+iCode;
	}
}
