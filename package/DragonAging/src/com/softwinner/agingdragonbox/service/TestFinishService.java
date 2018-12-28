package com.softwinner.agingdragonbox.service;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.SystemProperties;
import android.util.Log;

import com.softwinner.agingdragonbox.ThreadVar;
import com.softwinner.agingdragonbox.engine.Utils;

public class TestFinishService extends Service{
    public static String TAG ="DragonAging-TestFinishService";
    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }
    
    @Override
    public int onStartCommand(Intent intent,int flags, int startId) {
        Log.e(TAG,"dragonaging aging time up!!!");  
        ThreadVar.testFinished = true;
        SystemProperties.set(Utils.PROPERTY_DRAGONAGING,"false");
        //SystemProperties.set(Utils.PROPERTY_DRAGONAGING_TIME,"0");
        return super.onStartCommand(intent, flags, startId);
    }
}
