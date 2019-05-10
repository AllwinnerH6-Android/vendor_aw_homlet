package com.softwinner.dragonbox.platform;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class PhaseCallBack implements IPhaseCallback {
	private static final String TAG = "DragonBox-H6-Android4.4-yst-PhaseCallBack";
	private static final String ACTION = "com.lhxk.voice_recognition_end";
	public void allResultTestPassCallback(Context context) {
		Log.w(TAG,"trigger allResultTestPassCallback!");
		try {
			Intent intent = new Intent();
			intent.setClassName("com.allwinnertech.dragonenter", "com.allwinnertech.dragonenter.EnterActivity");
			context.startActivity(intent);			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public void startAppCallback(Context context) {
		Log.w(TAG,"trigger startAppCallback");
		Intent intent = new Intent();
		intent.setAction(ACTION);
		context.sendBroadcast(intent);
	}
	public void allTestOverCallback(Context context,String...params) {
		Log.w(TAG,"trigger allTestOverCallback");
	}
}
