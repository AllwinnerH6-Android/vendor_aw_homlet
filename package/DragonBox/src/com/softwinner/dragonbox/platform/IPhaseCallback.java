package com.softwinner.dragonbox.platform;

import android.content.Context;

public interface IPhaseCallback {
	void allResultTestPassCallback(Context context);
	void allTestOverCallback(Context context,String...params);
	void startAppCallback(Context context);
}
