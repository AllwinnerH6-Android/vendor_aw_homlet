package com.softwinner.dragonbox;

import java.util.ArrayList;
import java.util.List;

import java.io.File;
import java.io.FileOutputStream;
import java.lang.Thread;
import java.net.URL;
import java.lang.reflect.Field;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.os.Build;
import android.provider.ContactsContract.CommonDataKinds;
import android.provider.MediaStore.Images.Thumbnails;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.content.Context;
import android.content.SharedPreferences; 
import android.content.SharedPreferences.Editor;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.app.AlertDialog.Builder;
import android.app.ProgressDialog;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.provider.Settings;

import com.softwinner.dragonbox.config.ConfigManager;
import com.softwinner.dragonbox.entity.NetReceivedResult;
import com.softwinner.dragonbox.jni.ReadPrivateJNI;
import com.softwinner.dragonbox.platform.IPhaseCallback;
import com.softwinner.dragonbox.testcase.CaseEthernet;
import com.softwinner.dragonbox.testcase.CasePerformance;
import com.softwinner.dragonbox.testcase.CaseSIM;
import com.softwinner.dragonbox.testcase.CaseWifi;
import com.softwinner.dragonbox.testcase.CaseYSTWan;
import com.softwinner.dragonbox.testcase.CaseYSTWifi;
import com.softwinner.dragonbox.testcase.IBaseCase;
import com.softwinner.dragonbox.utils.CommandUtil;
import com.softwinner.dragonbox.utils.NetUtil;
import com.softwinner.dragonbox.utils.PreferenceUtil;
import com.softwinner.dragonbox.utils.Utils;
import com.softwinner.dragonbox.utils.LogcatHelper;
import com.softwinner.SystemMix;
import com.softwinner.dragonbox.platform.PhaseCallBack;

public class DragonBoxMain extends Activity implements IBaseCase.onResultChangeListener{

	// public ViewGroup fullContainer;
	public LinearLayout caseContainner;
	private List<IBaseCase> mAllCases = new ArrayList<IBaseCase>();
	private List<IBaseCase> mAllAutoCases = new ArrayList<IBaseCase>();
	private List<IBaseCase> mAllManualCases = new ArrayList<IBaseCase>();

	private int backPressTimes = 0;
	private static final int BACK_PRESS_MAX_TIMES = 3;
	private int prePressNum = -1;
	
	private static final int WHAT_HANDLE_RESTART = 0;
	private static final int WHAT_HANDLE_KEY = 1;
	private static final int WHAT_HANDLE_FINISH = 2;
	private static final int WHAT_HANDLE_WIFI_CONNECT_TIMEOUT = 3;
	private static final int WHAT_HANDLE_CHECKSSN_NEW = 4;
	private static final int WHAT_HANDLE_SHOW_MESSAGE_PROGRESS=5;
	private static final int WHAT_HANDLE_SHOW_MESSAGE_DIALOG=6;
	private static final int WHAT_HANDLE_SAVESSN_NEW = 7;
	private static final int WHAT_HANDLE_SFCTESTRESULT_UPLOAD = 8;
	
	private static final int WIFI_CONNECT_TIMEOUT = 10*1000;//wifi 连接超时10s
	private static final int RETRY_CONNECT_SERVER_TIMES =5;//重连服务器次数。
	private static final String TIME_WEBSITE="http://www.mi.com";
	public static final String TAG="DragonBox-DragonBoxMain";
    public static final String LOG_END_FLAG="DragonBox-test_over";
    public static final String PROPERTY_DRAGONAGING_NEXTBOOT = "persist.sys.dragonaging";
	private static final String PROPERTY_DRAGONAGING_TIME = "persist.sys.dragonaging_time";
	private static final String PROPERTY_DRAGONBOX_SMT = "persist.sys.smt_dragonbox";
	public static final String PROPERTY_SMT_DRAGONBOX_TESTCASE = "persist.sys.dragonbox_case";
	public static final String PROPERTY_SMT_DRAGONBOX_TESTRESULT = "persist.sys.dragonbox_result";
	private static final String PROPERTY_DRAGONBOX_RMFILE = "dragonbox.sys.rmfile";
	private static final String DRAGONAGING_LOG = "/sdcard/ALLWINNERAGING";
	public String sSSN = null;
	private ProgressDialog progressDialog;
	private String mTestResult = "FAIL";
	private String mTestItems = "";
	private String mTestValues = "";
	private NetReceivedResult mCheckSSNResult =null;
	private NetReceivedResult mUploadResult =null;
	private NetReceivedResult mSaveSSNResult =null;
	private AlertDialog wifiConnectTimeoutDialog = null;
	private boolean useSFC =false;
    private boolean alreadyUpload = false;
    private IPhaseCallback mPhaseCallback = new PhaseCallBack();
	
	Handler mHandler = new Handler(){
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
			case WHAT_HANDLE_RESTART:
				restartAllTest();
				break;
			case WHAT_HANDLE_KEY:
				int positionNum = msg.arg1;
				if (positionNum > mAllCases.size() && positionNum >= 0){
					Toast.makeText(DragonBoxMain.this, R.string.case_main_alert_item_not_support, Toast.LENGTH_SHORT).show();
				} else {
					mAllCases.get(positionNum - 1).mActionView.performClick();
				}
				prePressNum = -1;
				break;
			case WHAT_HANDLE_FINISH:
				finish();
				break;
			case WHAT_HANDLE_WIFI_CONNECT_TIMEOUT:
				if(progressDialog.isShowing())
					progressDialog.dismiss();
				wifiConnectTimeoutDialog = new AlertDialog.Builder(DragonBoxMain.this)
						.setMessage(getString(R.string.WIFI_TIMEOUT_RETRY))
						.setPositiveButton(R.string.DIALOG_RETRY, new OnClickListener() {
							
							@Override
							public void onClick(DialogInterface dialog, int which) {
								// TODO Auto-generated method stub
								Log.e(TAG, "retry to connect wifi!");
								NetUtil.connectWifi(DragonBoxMain.this, CaseWifi.SSID, CaseWifi.password);
								sendEmptyMessageDelayed(WHAT_HANDLE_WIFI_CONNECT_TIMEOUT, WIFI_CONNECT_TIMEOUT);
								progressDialog.setMessage(getString(R.string.PROGRESS_DIALOG_MESSAGE));
								progressDialog.show();
							}
						}).create();
				wifiConnectTimeoutDialog.show();
				break;
			case WHAT_HANDLE_CHECKSSN_NEW:
				if(progressDialog.isShowing())
					progressDialog.dismiss();
				if(mCheckSSNResult.sResult.equals("PASS")) {
					Log.e(TAG,"checkSSN_NEW pass,now start testing!");
					restartAllTest();
				}else {
					showAlertDialogToFinish("checkSSN_NEW failed: "+mCheckSSNResult.sReason);
					Log.e(TAG, "checkSSN_NEW failed: "+mCheckSSNResult.sReason);
					NetUtil.forgetWifi(DragonBoxMain.this);
					//mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 6*1000);
				}
				break;
			case WHAT_HANDLE_SHOW_MESSAGE_PROGRESS:
				if(progressDialog.isShowing())
					progressDialog.dismiss();
				progressDialog.setMessage((String) msg.obj);
				progressDialog.show();
				break;
			case WHAT_HANDLE_SHOW_MESSAGE_DIALOG:
				showAlertDialogToFinish((String) msg.obj);
				NetUtil.forgetWifi(DragonBoxMain.this);
				break;
			case WHAT_HANDLE_SFCTESTRESULT_UPLOAD:
				if(progressDialog.isShowing())
					progressDialog.dismiss();
				if(mUploadResult.sResult.equals("PASS")) {
					Log.e(TAG,"SFCTestResult_UPLOAD pass,now start SAVESSN_NEW!");
                    if(Utils.deleteFile(new File(DRAGONAGING_LOG))){
                        Log.d(TAG,"DragonAging log file delete success");
                    }else{
                        Log.d(TAG,"DragonAging log file delete fail");
                    }
					SAVESSNRunInThread();
				}else {
					showAlertDialogToFinish("上传测试结果失败!\nSFCTestResult_UPLOAD failed: "+mCheckSSNResult.sReason);
					Log.e(TAG, "SFCTestResult_UPLOAD failed: "+mCheckSSNResult.sReason);
					NetUtil.forgetWifi(DragonBoxMain.this);
					//mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 6*1000);
				}
				break;
			case WHAT_HANDLE_SAVESSN_NEW:
				if(progressDialog.isShowing())
					progressDialog.dismiss();
				if(mSaveSSNResult.sResult.equals("PASS")) {
					Log.e(TAG,"SAVESSN_NEW pass, test successfully!");
                    String otherOptionResult="";
                    otherOptionResult += "\n"+Utils.readFile(Utils.RID_FILE_PATH);
                    if(new File(DRAGONAGING_LOG).exists()){
                        otherOptionResult += "\nDragonAging日志：未删除!";
                        Log.e(TAG,"DragonAging日志：未删除");
                    }
                    Log.e(TAG,"assy "+LOG_END_FLAG);
					showAlertDialogToFinish(getString(R.string.test_success)+otherOptionResult);
				}else {
					showAlertDialogToFinish("SAVESSN_NEW failed: "+mCheckSSNResult.sReason);
					Log.e(TAG, "SAVESSN_NEW failed: "+mCheckSSNResult.sReason);
					//mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 8*1000);
				}
				NetUtil.forgetWifi(DragonBoxMain.this);
				break;
			default:
					break;
			}
		};
	};
	private void showAlertDialogToFinish(String msg) {
		AlertDialog alertDialog = new AlertDialog.Builder(DragonBoxMain.this)
				.setMessage(msg).create();
        alertDialog.setCancelable(false);
		alertDialog.show();
	}
	private void showAlertDialogToFinish(String msg,int size) {
		AlertDialog alertDialog = new AlertDialog.Builder(DragonBoxMain.this)
				.setMessage(msg).create();
        alertDialog.setCancelable(false);
		alertDialog.show();
        try {
            Field mAlert = AlertDialog.class.getDeclaredField("mAlert");
            mAlert.setAccessible(true);
            Object mAlertController = mAlert.get(alertDialog);
            //通过反射修改message字体大小和颜色
            Field mMessage = mAlertController.getClass().getDeclaredField("mMessageView");
            mMessage.setAccessible(true);
            TextView mMessageView = (TextView) mMessage.get(mAlertController);
            mMessageView.setTextSize(size);
            //mMessageView.setTextColor(Color.GREEN);
        } catch (IllegalAccessException e1) {
            e1.printStackTrace();
        } catch (NoSuchFieldException e2) {
            e2.printStackTrace();
        }
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
        LogcatHelper.getInstance(this).start();//DragonBox保存日志至/sdcard/
        new Thread(){
            public void run(){
                //add flag file to tell others 'I'm running'
                try{
                    String filePath = getCacheDir().getPath() + File.separator + "state";
                    File runningStat = new File(filePath);
                    if(!runningStat.exists()){
                        runningStat.createNewFile();
                    }
                    FileOutputStream fileOutputStream = new FileOutputStream(runningStat,false);
                    fileOutputStream.write("0".getBytes());
                    fileOutputStream.flush();
                    fileOutputStream.close();
                    Thread.sleep(2000);
                    fileOutputStream = new FileOutputStream(runningStat,false);
                    fileOutputStream.write("1".getBytes());
                    fileOutputStream.flush();
                    fileOutputStream.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            };
        }.start();
        mPhaseCallback.startAppCallback(DragonBoxMain.this);//tell others 'I'm running'
		caseContainner = (LinearLayout) findViewById(R.id.main_case_container);
		try {
			List<IBaseCase> cases = ConfigManager.getInstence(this)
					.parseConfig();
			mAllCases = cases;
			for (int i = 0; i < cases.size(); i++) {
				IBaseCase curCase = cases.get(i);
				curCase.addRusultChangeListener(this);
				TextView numTV = (TextView) curCase.mMinView.findViewById(R.id.case_min_num);
				if (numTV != null) {
					numTV.setText((i + 1) + "");
				}
				caseContainner.addView(curCase.mMinView);
				if (curCase.getType() == IBaseCase.TYPE_MODE_AUTO) {
					//curCase.startCaseforAuto();
					mAllAutoCases.add(curCase);
				} else if (curCase.getType() == IBaseCase.TYPE_MODE_MANUAL) {
					mAllManualCases.add(curCase);
				}
			}
		} catch (Exception e) {
			Toast.makeText(this, "ERROR,Please check config files", Toast.LENGTH_LONG).show();
			e.printStackTrace();
		}
		IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
		Log.e(TAG, "useSFC = "+ CaseWifi.useSFC);
		if(CaseWifi.useSFC == null || CaseWifi.useSFC.equals("false")) {
            Log.w(TAG,"begin to test--------SMT");
			useSFC = false;
            Utils.setPropertySecure(PROPERTY_DRAGONBOX_SMT,"start_test");
			restartAllTest();
		}else {
            String isDragonAgingTesting = SystemProperties.get(PROPERTY_DRAGONAGING_NEXTBOOT,"null");
            Log.e(TAG, "isDragonAgingTesting = "+ isDragonAgingTesting);
            if(isDragonAgingTesting.equals("false")){
                Log.w(TAG,"begin to test----------assy");
                useSFC = true;
                registerReceiver(wifiReceiver, intentFilter);
                readyToStartTest();
            }else if(isDragonAgingTesting.equals("true")){
                int agingTime = SystemProperties.getInt(PROPERTY_DRAGONAGING_TIME,0);
                Log.w(TAG,"已经老化"+Utils.formatTime(agingTime)+"\n"+"老化测试未完成!\n不能开始组装接口测试!\n"+"请将板子归类送至老化工站继续老化");
                showAlertDialogToFinish("已经老化"+Utils.formatTime(agingTime)+"\n"+"老化测试未完成!\n不能开始组装接口测试!\n"+"请将板子归类送至老化工站继续老化",30);
            }else if(isDragonAgingTesting.equals("null")){
                Log.w(TAG,"SMT未完成DragonBox测试!\n"+"老化测试未完成!\n不能开始组装接口测试!\n"+"请将板子归类送至SMT进行DragonBox测试");
                showAlertDialogToFinish("SMT未完成DragonBox测试!\n"+"老化测试未完成!\n不能开始组装接口测试!\n"+"请将板子归类送至SMT进行DragonBox测试",30);
            }else{
                Log.w(TAG,"DragonAging属性获取异常!value="+isDragonAgingTesting);
                showAlertDialogToFinish("DragonAging属性获取异常!value="+isDragonAgingTesting,30);
            }
		}
	}

	private BroadcastReceiver wifiReceiver = new BroadcastReceiver() {
		/*
		 * 检测到网络状态由connecting->Connected->Connected后,认定wifi连接成功，开始checkSSN。
		 * 暂时不清楚为什么后面有连续两个Connected状态。正常情况下只有一个Connected状态。
		 */
		boolean checkStateConnected = false;
		boolean checkStateConnecting = false;
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.e(TAG, "Receive wifi network state changed broadcast!!");
			if(intent.getAction().equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
				NetworkInfo networkInfo = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
				if(networkInfo!=null&&networkInfo.getExtraInfo().equals("\""+CaseWifi.SSID+"\"")) {
					Log.e(TAG, ""+networkInfo.getState());
					switch (networkInfo.getState()) {
					case CONNECTED:
						if(checkStateConnecting==true && checkStateConnected==false) {
							checkStateConnected=true;
						}else if(checkStateConnecting==true && checkStateConnected==true){
							checkStateConnected = false;//避免连接后，下次如果只接收到一个connected，就重新触发测试。
							checkStateConnecting = false;
							if(wifiConnectTimeoutDialog!=null&&wifiConnectTimeoutDialog.isShowing())
								wifiConnectTimeoutDialog.dismiss();//wifi 连接超时后，弹出超时对话框提示用户重新连接，之后如果
																	//用户没有点击重试，wifi又重新连接成功，此时隐藏提示对话框，开始测试。
							if(mHandler.hasMessages(WHAT_HANDLE_WIFI_CONNECT_TIMEOUT))
								mHandler.removeMessages(WHAT_HANDLE_WIFI_CONNECT_TIMEOUT);
							Log.e(TAG, "begin to check SSN!");
							progressDialog.setMessage(getString(R.string.BEGIN_CHECK_SSN));
							checkSSNinThread();
						}
						break;
					case CONNECTING:
						checkStateConnecting = true;
						checkStateConnected = false;
						break;
					default:
						checkStateConnected = false;
						checkStateConnecting = false;
						break;
					}
				}
			}
		}
	};
	
	private void checkSSNinThread() {
		new Thread() {
			@Override
			public void run() {
				super.run();
				for(int i=0;i<RETRY_CONNECT_SERVER_TIMES;i++) {
					Log.e(TAG, "url = "+CaseWifi.url);
					mCheckSSNResult = CommandUtil.CheckSSN_NEW(CaseWifi.url, sSSN, CaseWifi.station);
					if(mCheckSSNResult.iCode==200) {
						mHandler.sendEmptyMessage(WHAT_HANDLE_CHECKSSN_NEW);						
						return;
					}else {
						Log.e(TAG, "connect to server failed,retry after 1 second.");
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
				Log.e(TAG, "connect to server failed,finish DragonBox in 4s");
				Message message = mHandler.obtainMessage(WHAT_HANDLE_SHOW_MESSAGE_DIALOG, getString(R.string.CHECK_SSN_connect_server_failed));
				mHandler.sendMessage(message);
				mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 5*1000);
			}
		}.start();
	}
	
	
	private void readyToStartTest() {
		ReadPrivateJNI readPrivateJNI = new ReadPrivateJNI();
		sSSN = readPrivateJNI.nativeGetParameter("serialno");
		if(sSSN != null&&sSSN.length()>0) {
			Log.e(TAG, "SSN = "+sSSN);
			if(CaseWifi.SSID!=null&&CaseWifi.password!=null) {
				Log.e(TAG,"SSID = "+CaseWifi.SSID+" password = "+CaseWifi.password);
				progressDialog = new ProgressDialog(DragonBoxMain.this);
				progressDialog.setMessage(getString(R.string.PROGRESS_DIALOG_MESSAGE));
				progressDialog.show();
				NetUtil.connectWifi(this, CaseWifi.SSID, CaseWifi.password);
				mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_WIFI_CONNECT_TIMEOUT, WIFI_CONNECT_TIMEOUT);
			}else {
				showAlertDialogToFinish("wifi config err!");
				mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 5000);
			}
		}else {
			showAlertDialogToFinish(getString(R.string.SSN_ERR_FINISH));
			mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 5000);
		}
        
		//mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_RESTART, 100);
	}
	
	private void generateLinkCases(List<IBaseCase> cases) {
		if (cases == null || cases.size() == 0) {
			return;
		}
		int size = cases.size();
		for (int i = 0; i < size - 1; i++) {
			cases.get(i).setNextCase(cases.get(i + 1));
		}
		cases.get(0).mActionView.performClick();
	}

	interface onFullWindowAct {
		void onFullWindowShow();
		void onFullWindowHide();
	}

	public void cancelAllTest(){
		for (IBaseCase baseCase : mAllCases) {
			baseCase.stopCase();
			baseCase.cancel();
		}
	}
	
	public void restartAllTest() {
		boolean haveEthCase = false;
		IBaseCase caseYSTWifi = null;
		IBaseCase caseYSTWan = null;
		IBaseCase caseSIM = null;
        mTestItems = "";
        mTestValues = "";
		for (IBaseCase baseCase : mAllCases) {
			baseCase.mFinishTest = false;
			if (baseCase instanceof CaseEthernet || baseCase instanceof CaseYSTWan) {
				haveEthCase = true;
			}
			if (baseCase instanceof CaseYSTWifi) {
				caseYSTWifi = baseCase;
			}
			if (baseCase instanceof CaseYSTWan) {
				caseYSTWan = baseCase;
			}
			if(baseCase instanceof CaseSIM) {
				caseSIM = baseCase;
			}
			baseCase.stopCase();
			baseCase.reset();
		}
		
		for (IBaseCase baseCase : mAllAutoCases) {
			
			if (baseCase instanceof CaseWifi && haveEthCase) {
				baseCase.startCaseDelay(1000);//之前这里延时6s，可能是为了放至和以太网测试存在冲突，现在国腾M客户没有以太网测试，所以降低延时
			}else if (baseCase instanceof CaseYSTWifi) {
				baseCase.startCastAfterCase(caseYSTWan);
			}else if (baseCase instanceof CaseYSTWan) {
				baseCase.startCastAfterCase(caseYSTWifi);
			}else if(baseCase instanceof CaseWifi) {//sim卡联网测试在wifi之前，保证sim卡联网时是通过4G出去的
				baseCase.startCastAfterCase(caseSIM);
			} else {
				baseCase.startCase();
			}
		}
		generateLinkCases(mAllManualCases);
	}

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
                backPressTimes++;
                if (backPressTimes >= BACK_PRESS_MAX_TIMES) {
                    try{
                        String filePath = getCacheDir().getPath() + File.separator + "state";
                        File runningStat = new File(filePath);
                        if(!runningStat.exists()){
                            runningStat.createNewFile();
                        }
                        FileOutputStream fileOutputStream = new FileOutputStream(runningStat,false);
                        fileOutputStream.write("0".getBytes());
                        fileOutputStream.flush();
                        fileOutputStream.close();
                    }catch(Exception e){
                        e.printStackTrace();
                    }
                    finish();
                    Runtime.getRuntime().exit(0);
                } else {
                    Toast.makeText(this, "再按两次返回键退出应用", Toast.LENGTH_SHORT).show();
                }
                return true;
            case KeyEvent.KEYCODE_0:
            case KeyEvent.KEYCODE_1:
            case KeyEvent.KEYCODE_2:
            case KeyEvent.KEYCODE_3:
		case KeyEvent.KEYCODE_4:
		case KeyEvent.KEYCODE_5:
		case KeyEvent.KEYCODE_6:
		case KeyEvent.KEYCODE_7:
		case KeyEvent.KEYCODE_8:
		case KeyEvent.KEYCODE_9:
			mHandler.removeMessages(WHAT_HANDLE_KEY);
			Message msg = mHandler.obtainMessage();
			msg.what = WHAT_HANDLE_KEY;
			if (prePressNum == -1) {
				prePressNum = keyCode - KeyEvent.KEYCODE_0;
				msg.arg1 = prePressNum;
				if (keyCode == KeyEvent.KEYCODE_0 || keyCode == KeyEvent.KEYCODE_1) {
					mHandler.sendMessageDelayed(msg, 1000);					
				} else {
					mHandler.sendMessage(msg);
				}
			} else {
				msg.arg1 = prePressNum * 10 + (keyCode - KeyEvent.KEYCODE_0);
				mHandler.sendMessage(msg);
			}
			backPressTimes = 0;
			return true;
        case KeyEvent.KEYCODE_MENU:
            showMenuDialog();
            //return true; 暂时不return true,还未查明onCreateOptionsMenu为什么没有执行.
		default:
			break;
		}
        
		backPressTimes = 0;
		return false;
	}
    
    public void showMenuDialog(){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("请选择:");
        String[] operations = {"重新测试","启动DragonSN","启动DragonAging"};
        builder.setSingleChoiceItems(operations,1,new DialogInterface.OnClickListener(){
            @Override
            public void onClick(DialogInterface dialog,int which){
                dialog.dismiss();
                switch (which) {
                    case 0:
                        restartAllTest();
                        break;
                    case 1:
                        ConfigManager.startConfigAPK(DragonBoxMain.this, ConfigManager.CONFIG_DRAGON_SN, true);
                        break;
                    case 2:
                        ConfigManager.startConfigAPK(DragonBoxMain.this, ConfigManager.CONFIG_DRAGON_AGING, true);
                        break;
                }
            }
        });
        builder.show();
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main_menu, menu);
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		return super.onPrepareOptionsMenu(menu);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		super.onOptionsItemSelected(item);
		switch (item.getItemId()) {
		case R.id.menu_retest:
			restartAllTest();
			break;
		case R.id.menu_go_dragonsn:
			ConfigManager.startConfigAPK(this, ConfigManager.CONFIG_DRAGON_SN, true);
			break;
		case R.id.menu_go_dragonaging:
			ConfigManager.startConfigAPK(this, ConfigManager.CONFIG_DRAGON_AGING, true);
			break;
		}
		return true;
	}

	@Override
	public void onResultChange(IBaseCase baseCase, boolean caseResult) {
		/*for (IBaseCase cases : mAllCases) {
			if (!cases.getCaseResult()){
				if (PreferenceUtil.readFactory(this) != PreferenceUtil.FACTORY_FAIL) {
					PreferenceUtil.writeFactory(this,false);					
				}
				return;
			}
		}
		PreferenceUtil.writeFactory(this,true);
		ConfigManager.startConfigAPK(this, ConfigManager.CONFIG_DRAGON_SN, false);*/
        if(!useSFC){
            boolean wifiNotFinishTest = false;
            int finishedTestCount = 0;
            for(IBaseCase icase:mAllCases){
                if(icase.mFinishTest)
                    finishedTestCount++;
                if((icase instanceof CaseWifi)&&(!icase.mFinishTest)){
                    wifiNotFinishTest = true;
                }
            }
            if((finishedTestCount==mAllCases.size()-1)&&wifiNotFinishTest){
               progressDialog = new ProgressDialog(DragonBoxMain.this);
               progressDialog.setMessage("等待wifi测试结果");
               progressDialog.show();
            }
        }
		for(IBaseCase cases : mAllCases) {//判断是否完成所有测试项
			if(!cases.mFinishTest)
				return;
		}
        if(!useSFC){
            if(progressDialog!=null&&progressDialog.isShowing()){
                progressDialog.dismiss();
            }
        }

        int userSetupComplete = Settings.Secure.getInt(getContentResolver(),"user_setup_complete",0);
        Log.d(TAG,"判断是否进入主界面:user Setup Complete = "+userSetupComplete);
        if(userSetupComplete == 1){
            Utils.setPropertySecure(PROPERTY_DRAGONBOX_RMFILE,"/data/system/users/0/settings_secure.xml");
            Utils.setPropertySecure(PROPERTY_DRAGONBOX_RMFILE,"/data/system/users/0/package-restrictions.xml");
        }

		mTestResult = "PASS";
        Log.e(TAG,"original mTestItems = "+mTestItems);
        mTestItems = "";//此处会概率性连续调用两次，为了保证上传字符串不会太长，此处再次初始化
        mTestValues = "";
		for(IBaseCase cases:mAllCases) {//判断所有测试项是否全部pass
			if(!cases.getCaseResult()) {
				mTestResult = "FAIL";
			}
			mTestItems += cases.mTestName+",";
            if(cases.mTestName.contains("Wifi")){
                mTestValues += cases.getCaseResult()?"P"+CaseWifi.wifiRssi+",":"F"+CaseWifi.wifiRssi+",";
            }else if(cases.mTestName.contains("Ver")){
                String display = Build.DISPLAY;
                String[] split = display.split("\\.");
                String version = split[split.length-1];
                mTestValues += cases.getCaseResult()?"P"+version+",":"F"+version+",";
            }else{
                if(cases.getCaseResult()){
                    mTestValues += "P,";
                }else{
                    mTestValues += "F,";
                }
            }
		}
        if(mTestItems.length()>0)
            mTestItems = mTestItems.substring(0, mTestItems.length()-1);//列举每个测试项，","分割
        if(mTestValues.length()>0)
            mTestValues = mTestValues.substring(0,mTestValues.length()-1);//列举每个测试项对应的结果,","分割

		Log.e(TAG, "mTestItems = "+mTestItems+"\n mTestValues = "+mTestValues);
        if(!useSFC) {
            Log.e(TAG,"before setproperty "+LOG_END_FLAG);//通知LogcatHelper强制将log写入磁盘
            mPhaseCallback.allTestOverCallback(DragonBoxMain.this,mTestItems,mTestValues);
            if(mTestResult.equals("PASS")) {
                mPhaseCallback.allResultTestPassCallback(DragonBoxMain.this);
            }
            NetUtil.forgetWifi(DragonBoxMain.this);
            Utils.setPropertySecure(PROPERTY_DRAGONBOX_SMT,"test_over");
        }
        Log.e(TAG,LOG_END_FLAG);
        if(!useSFC) {
            if(mTestResult.equals("PASS")){
                showAlertDialogToFinish("成功!",40);
            }else{
                showAlertDialogToFinish("失败!",40);
            }
            return;
        }

        if(alreadyUpload){//已经上传数据，不再上传
            Log.e(TAG,"alreadyUpload ,donot upload agagin!");
            return;
        }
		progressDialog.setMessage(getString(R.string.GET_NETWORK_TIME));
		progressDialog.show();
        alreadyUpload = true;
		SFCUploadRunInThread();
	}
	
	private void SFCUploadRunInThread() {
		new Thread() {
			@Override
			public void run() {
				super.run();
				String time = NetUtil.getWebsiteDatetime(CaseWifi.url);
				Message message = mHandler.obtainMessage(WHAT_HANDLE_SHOW_MESSAGE_PROGRESS, getString(R.string.SfcTestResult_Upload));
				mHandler.sendMessage(message);
				for(int i=0;i<RETRY_CONNECT_SERVER_TIMES;i++) {
					Log.e(TAG, "SfcTestResult_Upload: ready to connect url: "+CaseWifi.url);
					mUploadResult = CommandUtil.SfcTestResult_Upload(CaseWifi.url, CaseWifi.station,
							sSSN, mTestResult, time, mTestItems, mTestValues, CaseWifi.pcname);
					if(mUploadResult.iCode==200) {
						mHandler.sendEmptyMessage(WHAT_HANDLE_SFCTESTRESULT_UPLOAD);						
						return;
					}else {
						Log.e(TAG, "SfcTestResult_Upload:connect to server failed,retry after 1 second.");
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
				Log.e(TAG, "connect to server failed,finish DragonBox in 4s");
				message = mHandler.obtainMessage(WHAT_HANDLE_SHOW_MESSAGE_DIALOG, getString(R.string.SfcTestResult_Upload_connect_server_failed));
				mHandler.sendMessage(message);
				mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 5*1000);
			}
		}.start();
	}
	
	private void SAVESSNRunInThread() {
		new Thread() {
			@Override
			public void run() {
				super.run();
				Message message = mHandler.obtainMessage(WHAT_HANDLE_SHOW_MESSAGE_PROGRESS, getString(R.string.SAVESSN_NEW));
				mHandler.sendMessage(message);
				for(int i=0;i<RETRY_CONNECT_SERVER_TIMES;i++) {
					Log.e(TAG, "SAVESSN_NEW: ready to connect url: "+CaseWifi.url);
					mSaveSSNResult = CommandUtil.SaveSSN_NEW(CaseWifi.url, sSSN, CaseWifi.station, mTestResult, mTestResult, CaseWifi.pcname);
					if(mSaveSSNResult.iCode==200) {
						mHandler.sendEmptyMessage(WHAT_HANDLE_SAVESSN_NEW);						
						return;
					}else {
						Log.e(TAG, "SAVESSN_NEW:connect to server failed,retry after 1 second.");
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
				Log.e(TAG, "connect to server failed,finish DragonBox in 4s");
				message = mHandler.obtainMessage(WHAT_HANDLE_SHOW_MESSAGE_DIALOG, getString(R.string.SAVE_SSN_connect_server_failed));
				mHandler.sendMessage(message);
				mHandler.sendEmptyMessageDelayed(WHAT_HANDLE_FINISH, 5*1000);
			}
		}.start();
	}

    @Override
    protected void onDestroy() {
    	unregisterReceiver(wifiReceiver);
        super.onDestroy();
    }
	
	/*
	 * void showFullWindow(View view){ fullContainer.removeAllViews();
	 * fullContainer.addView(((IBaseCase) view.getTag()).mMaxView);
	 * fullContainer.setVisibility(View.VISIBLE); }
	 * 
	 * private void hideFullWindow(){ if (fullContainer.getChildCount() > 0) {
	 * ((IBaseCase) fullContainer.getChildAt(0).getTag()).stopCase(); }
	 * fullContainer.removeAllViews(); fullContainer.setVisibility(View.GONE); }
	 */
	/*
	 * @Override public void onClick(View view) { showFullWindow(view);
	 * ((IBaseCase) view.getTag()).startCaseforAuto(); }
	 */

}
