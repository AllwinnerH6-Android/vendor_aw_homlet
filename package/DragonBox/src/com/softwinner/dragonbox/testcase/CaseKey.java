package com.softwinner.dragonbox.testcase;

import org.xmlpull.v1.XmlPullParser;

import com.softwinner.dragonbox.R;

import android.R.xml;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.TextView;

public class CaseKey extends IBaseCase {
	private final static String TAG="CaseKey"; 
	private final static int MSG_KEY_TEST_END = 0;
	TextView mTvMaxKeyInfo;
	TextView mTvMinKeyStatus;
	int mTestKeyValue=0;
	Context mContext;
	Handler mKeyHandler = new Handler(Looper.myLooper()) {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
			case MSG_KEY_TEST_END:
				mMaxViewDialog.dismiss();
				break;

			default:
				break;
			}
		};
	};
	public CaseKey(Context context) {
		super(context, R.string.case_key_name, R.layout.case_key_max, R.layout.case_key_min, TYPE_MODE_MANUAL);
		mContext = context;
		mTvMaxKeyInfo = (TextView) mMaxView.findViewById(R.id.case_key_info);
		mTvMinKeyStatus = (TextView) mMinView.findViewById(R.id.case_key_status);
		setShowDialogButton(false);
	}
	
	public CaseKey(Context context, XmlPullParser xmlParser) {
		this(context);
		String keyValue = xmlParser.getAttributeValue(null,"keyvalue");
		mTestKeyValue = KeyEvent.keyCodeFromString(keyValue);
		Log.d(TAG, "the test key value is "+KeyEvent.keyCodeToString(mTestKeyValue));
	}
	
	@Override
	public void onStartCase() {
		// TODO Auto-generated method stub
		mTvMaxKeyInfo.setText(mContext.getString(R.string.case_key_info, KeyEvent.keyCodeToString(mTestKeyValue)));
		mTvMaxKeyInfo.setFocusable(true);
		setDialogPositiveButtonEnable(false);
		mTvMaxKeyInfo.setOnKeyListener(new OnKeyListener() {
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				Log.d(TAG,"key down code is "+keyCode);	
				if(keyCode == mTestKeyValue) {
					setCaseResult(true);
					mTvMaxKeyInfo.setText(mContext.getString(R.string.case_key_detect_success, KeyEvent.keyCodeToString(keyCode)));
					mKeyHandler.sendEmptyMessageDelayed(MSG_KEY_TEST_END, 850);
				}else if(keyCode == KeyEvent.KEYCODE_DPAD_CENTER){
					setCaseResult(false);
					mTvMaxKeyInfo.setText(mContext.getString(R.string.case_key_detect_exit));
					mKeyHandler.sendEmptyMessageDelayed(MSG_KEY_TEST_END, 850);
				}else {
					mTvMaxKeyInfo.setText(mContext.getString(
							R.string.case_key_detect_error, KeyEvent.keyCodeToString(keyCode),KeyEvent.keyCodeToString(mTestKeyValue)));
				}
				return true;
			}
		});
	}

	@Override
	public void onStopCase() {
		// TODO Auto-generated method stub
		mTvMinKeyStatus
		.setText(getCaseResult() ? R.string.case_key_status_success_text
				: R.string.case_key_status_fail_text);
	}
	
	@Override
	public void reset() {
		super.reset();
	}
	

}
