package com.softwinner.dragonbox.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import com.softwinner.dragonbox.entity.NetReceivedResult;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.wifi.ScanResult;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiManager;
import android.util.Log;

public class NetUtil {
	public static final String TAG="DragonBox-NetUtil"; 
	public static NetReceivedResult getURLContentByPost(String urlStr, String sCommand,String[] arrSParams) {	
		String msg = "";
		NetReceivedResult netReceivedResult = new NetReceivedResult();
		try {
			URL url = new URL(urlStr);
			HttpURLConnection conn = (HttpURLConnection) url.openConnection();
			conn.setDoOutput(true);//设置是否向conn输出，因为这个是post请求，参数需要放在http正文内，需要设为TRUE，默认false
			conn.setDoInput(true);//设置是否从conn读取，默认true。
			conn.setUseCaches(false);//post请求不能使用缓存
			conn.setConnectTimeout(5*1000);
			conn.setReadTimeout(5*1000);
			conn.setInstanceFollowRedirects(true);//设置是否自动执行http重定向。
			conn.setRequestProperty("Content-Type", "text/xml; charset=utf-8");
			conn.setRequestProperty("SOAPAction", "http://tempuri.org/"+sCommand);
			conn.setRequestMethod("POST");		
			conn.connect();//连接
			
			BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(conn.getOutputStream()));
			bw.write(SOAPUtil.getXml(sCommand, arrSParams));
			bw.flush();
			bw.close();
			
			int code = conn.getResponseCode();
			Log.e(TAG,"code = "+code);
			netReceivedResult.iCode=code;
			if(code==200) {
				InputStream inputStream = conn.getInputStream();
				XmlUtil.parseXML(inputStream,netReceivedResult);
				/*BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
				String line = "";
				while ((line = br.readLine())!=null) {
					msg += line+"\n";
				}
				br.close();*/
				inputStream.close();
			}else{
				Log.e(TAG,"error code = "+code);
				BufferedReader br = new BufferedReader(new InputStreamReader(conn.getErrorStream()));
				String line = "";
				Log.e(TAG,"msg = " + br.readLine());
				while ((line = br.readLine())!=null) {
					msg += line+"\n";
				}
				Log.e(TAG,"msg = "+msg);
				br.close();
			}
			
			conn.disconnect();
			//Log.e(TAG,"msg = "+msg);
			return netReceivedResult;
			
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return netReceivedResult;
	}

	public static void connectWifi(Context context,String ssid,String password) {
		WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
		if(!wifiManager.isWifiEnabled())
			wifiManager.setWifiEnabled(true);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		Log.e(TAG,"check ssid whether already connected: "+wifiInfo.getSSID());
		if(wifiInfo!=null&&wifiInfo.getSSID().equals("\""+ssid+"\"")&&wifiInfo.getSupplicantState()==SupplicantState.COMPLETED) {
			Log.e(TAG,"Wifi has connected the target ssid:"+ssid);
			wifiManager.disconnect();
			//return ;
		}
		List<WifiConfiguration> lstWifiConfiguration = wifiManager.getConfiguredNetworks();
		if (lstWifiConfiguration != null && lstWifiConfiguration.size() > 0) {
			for (WifiConfiguration wifiConfig : lstWifiConfiguration) {
				wifiManager.removeNetwork(wifiConfig.networkId);
			}
		}
		WifiConfiguration wifiConfiguration = new WifiConfiguration();
		wifiConfiguration.SSID="\""+ssid+"\"";//小写的L
		wifiConfiguration.preSharedKey="\""+password+"\"";
		int netId = wifiManager.addNetwork(wifiConfiguration);
		wifiManager.enableNetwork(netId, true);
	}
	
    public static void forgetWifi(Context context) {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        List<WifiConfiguration> lstWifiConfiguration = wifiManager.getConfiguredNetworks();
        if (lstWifiConfiguration != null && lstWifiConfiguration.size() > 0) {
            for (WifiConfiguration wifiConfig : lstWifiConfiguration) {
                wifiManager.forget(wifiConfig.networkId,null);
            }
        }
    }
	
	public static String getWebsiteDatetime(String timeUrl) {
		try {
			URL url = new URL(timeUrl);
			URLConnection uc = url.openConnection();
			uc.connect();
			long ld = uc.getDate();
			Date date = new Date(ld);
			//上传年月日时分秒的格式不能被服务器解析，但年月日能被服务器解析。
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd", Locale.CHINA);
			return sdf.format(date);
		} catch (Exception e) {
			// TODO: handle exception
		}
		return "1970-01-01 00:00:00";
	}
}
