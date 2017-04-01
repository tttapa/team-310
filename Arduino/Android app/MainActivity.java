package testproject.test;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import static android.widget.Toast.LENGTH_SHORT;




public class MainActivity extends AppCompatActivity {
    //checks if the app is sending motion messages (button pressed)
    String sendingMotion = "0";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
///////////////////////////////////////////////////////////////////////
///////////////packet sending configuration////////////////////////////
//////////////////////////////////////////////////////////////////////
        final UDPSender sender = new UDPSender();
        sender.host = "www.google.com";
        sender.port = 80;
        sender.sendingInterval = 100;


/////////////////////////////////////////////////////////////////////
////////////////////Color configuration//////////////////////////////
/////////////////////////////////////////////////////////////////////
        final String pressColor = "#F7E0FA";
        final String releaseColor = "#ECBE06";

/////////////////////////////////////////////////////////////////////
///////////////////MOTION BUTTONS////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////------------------------FORWARD-------------------------------//////
        final Button UpButton = (Button) findViewById(R.id.Up);

        UpButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "forward";
                    //System.out.println("Button down");
                    sender.running = true;
                    UpButton.setTextColor(Color.BLACK);
                    UpButton.setBackgroundColor(Color.parseColor(pressColor));
                    //TODO: need to set specific data for "up" to send
                    sender.send(commands.FORWARD); // sending "up" to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("forward")) {
                    //System.out.println("button up");
                    sendingMotion = "0";
                    sender.running = false;
                    UpButton.setTextColor(Color.BLACK);
                    UpButton.setBackgroundColor(Color.parseColor(releaseColor));
                    return true;
                }

                return false;
            }
        });
//////------------------------DOWN-----------------------------//////////

        final Button DownButton = (Button) findViewById(R.id.Down);

        DownButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "down";
                    sender.running = true;
                    //TODO: need to set specific data for "down" to send
                    sender.send(commands.BACKWARD); // sending "down" to address defined at beginning
                    DownButton.setTextColor(Color.BLACK);
                    DownButton.setBackgroundColor(Color.parseColor(pressColor));
                    DownButton.setShadowLayer(20, 20, 20, Color.BLACK);
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("down")) {
                    sendingMotion = "0";
                    sender.running = false;
                    DownButton.setTextColor(Color.BLACK);
                    DownButton.setBackgroundColor(Color.parseColor(releaseColor));
                    DownButton.setShadowLayer(0, 0, 0, Color.BLACK);
                    return true;
                }

                return false;
            }
        });
////////----------------------LEFT-------------------------------//////////
        final Button LeftButton = (Button) findViewById(R.id.Left);

        LeftButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "left";
                    sender.running = true;
                    //TODO: need to set specific data for "left" to send
                    sender.send(commands.LEFT); // sending "left" to address defined at beginning
                    LeftButton.setTextColor(Color.BLACK);
                    LeftButton.setBackgroundColor(Color.parseColor(pressColor));
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("left")) {
                    sendingMotion = "0";
                    sender.running = false;
                    LeftButton.setTextColor(Color.BLACK);
                    LeftButton.setBackgroundColor(Color.parseColor(releaseColor));
                    return true;
                }

                return false;
            }
        });
////////------------------------RIGHT---------------------------///////////
        final Button RightButton = (Button) findViewById(R.id.Right);

        RightButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "right";
                    sender.running = true;
                    //TODO: need to set specific data for "right" to send
                    sender.send(commands.RIGHT); // sending "right" to address defined at beginning
                    RightButton.setTextColor(Color.BLACK);
                    RightButton.setBackgroundColor(Color.parseColor(pressColor));
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("right")) {
                    sendingMotion = "0";
                    sender.running = false;
                    RightButton.setTextColor(Color.BLACK);
                    RightButton.setBackgroundColor(Color.parseColor(releaseColor));
                    return true;
                }
                return false;
            }
        });
    }

}
