package uchile.eolianfenix;

import android.content.Intent;
import android.graphics.Color;
import android.media.Image;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.github.anastr.speedviewlib.SpeedView;
import com.ntt.customgaugeview.library.GaugeView;
import java.util.Random;
import android.os.CountDownTimer;

import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
//import android.widget.EditText;
import android.widget.TextView;
//import java.util.*;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;

//import android.os.Bundle;
//import android.support.design.widget.FloatingActionButton;
//import android.support.design.widget.Snackbar;
//import android.view.View;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
//import android.support.v7.app.AppCompatActivity;
//import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

//import android.view.Window;
import android.view.WindowManager;



public class MainActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {

    public final String ACTION_USB_PERMISSION = "uchile.serial_test.USB_PERMISSION";

    //Button startButton, stopButton;
    //TextView textView;
    //EditText editText;
    UsbManager usbManager;
    UsbDevice device;
    UsbSerialDevice serialPort;
    UsbDeviceConnection connection;
    String data;
    String name;
    String value;
    String name_aux;
    String value_aux;
    int velocidad_valor_int;
    float velocidad_valor_float;
    float soc_float;
    float mppt_power_float;
    float velocidad_float;
    double global_corriente;
    double global_voltaje;
    double global_potencia;

    public void destiny(List<StringTokenizer> args) {
        TextView Serial_input = (TextView) findViewById(R.id.serial_input);
        TextView vel_invisible_tv= (TextView) findViewById(R.id.velocidad_invisible);

        GaugeView gaugeView_speed = (GaugeView) findViewById(R.id.gauge_view_speed);        //a
        GaugeView gaugeView_soc = (GaugeView) findViewById(R.id.gauge_view_soc);            //b
        TextView bms_current_tv= (TextView) findViewById(R.id.bms_pack_current_valor);      //c
        TextView bms_pack_vtg_tv = (TextView) findViewById(R.id.bms_pack_vtg_valor);        //d
        GaugeView gaugeView_potencia = (GaugeView) findViewById(R.id.gauge_view_potencia);  //e
        TextView bms_min_volt_tv = (TextView) findViewById(R.id.bms_min_vtg_valor);          //f
        TextView bms_max_volt_tv = (TextView) findViewById(R.id.bms_max_vtg_valor);          //g
        TextView motor_izq_temp_tv = (TextView) findViewById(R.id.motor_temp_izq_valor);    //h
        TextView motor_der_temp_tv = (TextView) findViewById(R.id.motor_temp_der_valor);    //i
        TextView bms_min_temp_tv= (TextView) findViewById(R.id.bms_min_temp_valor);         //j
        TextView bms_max_temp_tv= (TextView) findViewById(R.id.bms_max_temp_valor);        //k
        TextView potencia_consumida_tv= (TextView) findViewById(R.id.potencia_consumida);   //BONUS


        StringTokenizer nombre = args.get(0);
        StringTokenizer valor = args.get(1);

        while (valor.hasMoreTokens()) {
            value = valor.nextToken();
            value_aux = value;
        }

        while (nombre.hasMoreTokens()) {
            name = nombre.nextToken();
            name_aux = name;

            tvAppend(Serial_input, name_aux+" "+value_aux);

            //Definitivos

            String velocidad = "a";
            String soc = "b";
            String corriente = "c";
            String voltaje = "d";
            String mppt_power = "e";
            String bms_min_volt = "f";
            String bms_max_volt = "g";
            String motor_izq_temp = "h";
            String motor_der_temp = "i";
            String bms_min_temp = "j";
            String bms_max_temp = "k";


            if (name.compareTo(velocidad) == 0) {                                   //a
                /*
                tvAppend(vel_invisible_tv, value);
                String rpm = vel_invisible_tv.getText().toString();
                revpm = Integer.parseInt(rpm);
                velocidad_valor_int = (int) ((double)revpm*(double)0.101410611); // considerando radio
                velocidad_valor_float = (float) ((double)revpm*(double)0.101410611); // considerando radio
                //refresh_speedometer(speedView, velocidad_valor_int);
                */
                velocidad_float = (float) Integer.parseInt(value);
                refresh_gauge(gaugeView_speed, velocidad_valor_float);
            }
            if (name.compareTo(soc) == 0) {                                         //b
                soc_float = (float) Integer.parseInt(value);
                refresh_gauge(gaugeView_soc, soc_float);
            }
            if (name.compareTo(corriente) == 0) {                                   //c
                tvAppend(bms_current_tv, value);
                global_corriente=(double) Integer.parseInt(value);
                global_potencia = (double) global_corriente*global_voltaje*0.001;   //BONUS Potencia en KW
                tvAppend(potencia_consumida_tv, Double.toString(global_potencia));
            }
            if (name.compareTo(voltaje) == 0) {                                     //d
                tvAppend(bms_pack_vtg_tv, value);
                global_voltaje = (double) Integer.parseInt(value);
                global_potencia = (double) global_corriente*global_voltaje*0.001;   //BONUS Potencia en KW
                tvAppend(potencia_consumida_tv, Double.toString(global_potencia));
            }
            if (name.compareTo(mppt_power) == 0) {                                  //e
                double potencia_generada = Integer.parseInt(value)*0.001;           // Potencia generada en KW
                mppt_power_float = (float) potencia_generada;
                refresh_gauge(gaugeView_potencia, velocidad_valor_float);
            }
            if (name.compareTo(bms_min_volt) == 0) {                                //f
                tvAppend(bms_min_volt_tv, value);
            }
            if (name.compareTo(bms_max_volt) == 0) {                                //g
                tvAppend(bms_max_volt_tv, value);
            }
            if (name.compareTo(motor_izq_temp) == 0) {                              //h
                tvAppend(motor_izq_temp_tv, value);
            }
            if (name.compareTo(motor_der_temp) == 0) {                              //i
                tvAppend(motor_der_temp_tv, value);
            }
            if (name.compareTo(bms_min_temp) == 0) {                                //j
                tvAppend(bms_min_temp_tv, value);
            }
            if (name.compareTo(bms_max_temp) == 0) {                                //k
                tvAppend(bms_max_temp_tv, value);
            }
        }

    }

    UsbSerialInterface.UsbReadCallback mCallback = new UsbSerialInterface.UsbReadCallback() { //Defining a Callback which triggers whenever data is read.
        @Override


        public void onReceivedData(byte[] arg0) {
            try {
                data = new String(arg0, "UTF-8");
                data = data.concat("");


                StringTokenizer tokens=new StringTokenizer(data, ",0123456789");
                StringTokenizer tokens1=new StringTokenizer(data, "!#$%&/()=?¡¨*[];:_,.-{}´+'¿^`~°|¬qwertyuiopasdfghjklñzxcvbnmQWERTYUIOPASDFGHJKLÑZXCVBNM"+"\n"+"\b");

                List<StringTokenizer> a = new ArrayList<>();
                a.add(0, tokens);
                a.add(1, tokens1);

                destiny(a);

                a.clear();

            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }


        }
    };
    private final BroadcastReceiver broadcastReceiver = new BroadcastReceiver() { //Broadcast Receiver to automatically start and stop the Serial connection.
        @Override
        public void onReceive(Context context, Intent intent) {
            //ImageView logo = (ImageView) findViewById(R.id.logo_top);
            //SpeedView speedView = (SpeedView) findViewById(R.id.speedView);
            GaugeView gaugeView_speed = (GaugeView)   findViewById(R.id.gauge_view_speed);
            //Button startButton = (Button)   findViewById(R.id.buttonStart);

            if (intent.getAction().equals(ACTION_USB_PERMISSION)) {

                TextView Serial_input = (TextView) findViewById(R.id.serial_input);
                boolean granted = intent.getExtras().getBoolean(UsbManager.EXTRA_PERMISSION_GRANTED);
                if (granted) {
                    connection = usbManager.openDevice(device);
                    serialPort = UsbSerialDevice.createUsbSerialDevice(device, connection);
                    if (serialPort != null) {
                        if (serialPort.open()) { //Set Serial Connection Parameters.
                            setUiEnabled(true);
                            serialPort.setBaudRate(57600);
                            serialPort.setDataBits(UsbSerialInterface.DATA_BITS_8);
                            serialPort.setStopBits(UsbSerialInterface.STOP_BITS_1);
                            serialPort.setParity(UsbSerialInterface.PARITY_NONE);
                            serialPort.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF);
                            serialPort.read(mCallback);
                            tvAppend(Serial_input,"Serial Connection Opened!\n");

                        } else {
                            Log.d("SERIAL", "PORT NOT OPEN");
                        }
                    } else {
                        Log.d("SERIAL", "PORT IS NULL");
                    }
                } else {
                    Log.d("SERIAL", "PER NOT GRANTED");
                }
            } else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)) {
                    onClickStart(gaugeView_speed.callOnClick());
                // Esto era onCLickStart(startButton)
            //} else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED)) {
                //onClickStop(stopButton);

            }
        }
    };





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.content_main);
        usbManager = (UsbManager) getSystemService(this.USB_SERVICE);
        //startButton = (Button) findViewById(R.id.buttonStart);
        //sendButton = (Button) findViewById(R.id.buttonSend);
        //clearButton = (Button) findViewById(R.id.buttonClear);
        //stopButton = (Button) findViewById(R.id.buttonStop);
        //editText = (EditText) findViewById(R.id.editText);
        //textView = (TextView) findViewById(R.id.textView1);
        setUiEnabled(false);
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_PERMISSION);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        registerReceiver(broadcastReceiver, filter);


        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN); //HIDE_NAVIGATION

        setContentView(R.layout.activity_main);

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);


        /////// GAUGE //////
        final GaugeView gaugeView_speed = (GaugeView) findViewById(R.id.gauge_view_speed);
        gaugeView_speed.setShowRangeValues(true);
        gaugeView_speed.setTargetValue(0);
        gaugeView_speed.setClickable(true);

        final GaugeView gaugeView_potencia = (GaugeView) findViewById(R.id.gauge_view_potencia);
        gaugeView_potencia.setShowRangeValues(true);
        gaugeView_potencia.setTargetValue(0);

        final GaugeView gaugeView_soc = (GaugeView) findViewById(R.id.gauge_view_soc);
        gaugeView_soc.setShowRangeValues(true);
        gaugeView_soc.setTargetValue(0);


        ////// SPEEDOMETER ///////////////////
        /*SpeedView speedView = (SpeedView) findViewById(R.id.speedView);

        speedView.setMaxSpeed(140);
        speedView.setSpeedTextSize(60);
        speedView.setSpeedTextColor(Color.rgb(232,232,232));
        speedView.setTextSize(30);
        speedView.setUnit("");
        speedView.setTextColor(Color.rgb(232,232,232));
        //speedView.setCenterCircleColor(Color.rgb(232,232,232));

        speedView.setBackgroundCircleColor(Color.rgb(26,60,101));    //Azul Color.rgb(26,60,101)
        speedView.setLowSpeedColor(Color.GREEN);         //Gris Color.rgb(232,232,232)
        speedView.setMediumSpeedColor(Color.YELLOW);
        speedView.setHighSpeedColor(Color.RED);
        speedView.setMarkColor(Color.rgb(232,232,232));
        //speedView.callOnClick();

        //speedView.setBackgroundColor(Color.BLACK);
        //speedView.setBackgroundCircleColor(Color.BLACK);

        speedView.speedTo(0); // Velocidad al iniciar el programa

        speedView.setWithTremble(false); // Velocimetro sin vibracion
*/

    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onResume(){
        super.onResume();
        View decorView = getWindow().getDecorView();
// Hide both the navigation bar and the status bar.
// SYSTEM_UI_FLAG_FULLSCREEN is only available on Android 4.1 and higher, but as
// a general rule, you should design your app to hide the status bar whenever you
// hide the navigation bar.
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY| View.SYSTEM_UI_FLAG_FULLSCREEN;
        decorView.setSystemUiVisibility(uiOptions);
    }


    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_camera) {
            // Handle the camera action
        } else if (id == R.id.nav_gallery) {

        } else if (id == R.id.nav_slideshow) {

        } else if (id == R.id.nav_manage) {

        } else if (id == R.id.nav_share) {

        } else if (id == R.id.nav_send) {

        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;

    }

    public void setUiEnabled(boolean bool) {
        TextView Serial_input = (TextView) findViewById(R.id.serial_input);
        TextView vel_invisible = (TextView) findViewById(R.id.velocidad_invisible);

        GaugeView gaugeView_speed = (GaugeView) findViewById(R.id.gauge_view_speed);        //a
        GaugeView gaugeView_soc = (GaugeView) findViewById(R.id.gauge_view_soc);            //b
        TextView bms_current_tv= (TextView) findViewById(R.id.bms_pack_current_valor);      //c
        TextView bms_pack_vtg_tv = (TextView) findViewById(R.id.bms_pack_vtg_valor);        //d
        GaugeView gaugeView_potencia = (GaugeView) findViewById(R.id.gauge_view_potencia);  //e
        TextView bms_min_volt_tv = (TextView) findViewById(R.id.bms_min_vtg_valor);          //f
        TextView bms_max_volt_tv = (TextView) findViewById(R.id.bms_max_vtg_valor);          //g
        TextView motor_izq_temp_tv = (TextView) findViewById(R.id.motor_temp_izq_valor);    //h
        TextView motor_der_temp_tv = (TextView) findViewById(R.id.motor_temp_der_valor);    //i
        TextView bms_min_temp_tv= (TextView) findViewById(R.id.bms_min_temp_valor);         //j
        TextView bms_max_temp_tv= (TextView) findViewById(R.id.bms_max_temp_valor);        //k
        TextView potencia_consumida_tv= (TextView) findViewById(R.id.potencia_consumida);   //BONUS

        Serial_input.setEnabled(bool);
        vel_invisible.setEnabled(bool);

        gaugeView_speed.setEnabled(bool);
        gaugeView_soc.setEnabled(bool);
        bms_current_tv.setEnabled(bool);
        bms_pack_vtg_tv.setEnabled(bool);
        gaugeView_potencia.setEnabled(bool);
        bms_min_volt_tv.setEnabled(bool);
        bms_max_volt_tv.setEnabled(bool);
        motor_izq_temp_tv.setEnabled(bool);
        motor_der_temp_tv.setEnabled(bool);
        bms_min_temp_tv.setEnabled(bool);
        bms_max_temp_tv.setEnabled(bool);
        potencia_consumida_tv.setEnabled(bool);

        //startButton.setEnabled(!bool);
        //sendButton.setEnabled(bool);
        //stopButton.setEnabled(bool);
    }

    //Esto era public void onClickStart(View view)
    public void onClickStart(boolean view) {

        HashMap<String, UsbDevice> usbDevices = usbManager.getDeviceList();
        if (!usbDevices.isEmpty()) {
            boolean keep = true;
            for (Map.Entry<String, UsbDevice> entry : usbDevices.entrySet()) {
                device = entry.getValue();
                int deviceVID = device.getVendorId();
                if (deviceVID == 0x2341)//Arduino Vendor ID
                {
                    PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
                    usbManager.requestPermission(device, pi);
                    keep = false;
                } else {
                    connection = null;
                    device = null;
                }

                if (!keep)
                    break;
            }
        }


    }
/*
    public void onClickSend(View view) {
        String string = editText.getText().toString();
        serialPort.write(string.getBytes());
        tvAppend(textView, "\nData Sent : " + string + "\n");
    }
*/
/*    public void onClickStop(View view) {
        TextView serial_input_tv = (TextView) findViewById(R.id.serial_input);

        setUiEnabled(false);
        serialPort.close();
        tvAppend(serial_input_tv,"\nSerial Connection Closed! \n");

    }
*/
/*    public void onClickClear(View view) {
        TextView serial_input_tv = (TextView) findViewById(R.id.serial_input);
        serial_input_tv.setText(" ");

    }
*/
    private void tvAppend(TextView tv, CharSequence text) {
        final TextView ftv = tv;
        final CharSequence ftext = text;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ftv.setText(null);
            }
        });
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ftv.append(ftext);
            }
        });
    }

    private void tvAppend_1(TextView tv, CharSequence text) {
        final TextView ftv = tv;
        final CharSequence ftext = text;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ftv.append(ftext);
            }
        });
    }

    public void refresh_speedometer(SpeedView speedviewer, int entero){
        final SpeedView spdvw = speedviewer;
        final int enterito = entero;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                spdvw.speedTo(enterito);
            }
        });
    }

    public void refresh_gauge(GaugeView gaugeviewer, float flotante){
        final float valor = flotante;
        final GaugeView gvw = gaugeviewer;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gvw.setTargetValue(valor);
            }
        });

    }

}
