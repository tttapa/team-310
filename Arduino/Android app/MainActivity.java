package testproject.test;

import android.content.Context;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import static android.widget.Toast.LENGTH_SHORT;




public class MainActivity extends AppCompatActivity {
    //checks if the app is sending motion messages (button pressed)
    String sendingMotion = "0";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);
///////////////////////////////////////////////////////////////////////
///////////////packet sending configuration////////////////////////////
//////////////////////////////////////////////////////////////////////
        final UDPSender sender = new UDPSender();
        sender.host = "www.google.com";
        sender.port = 80;
        sender.sendingInterval = 90;


/////////////////////////////////////////////////////////////////////
////////////////////Color configuration//////////////////////////////
/////////////////////////////////////////////////////////////////////
        final String pressColor = "#F7E0FA";
        final String releaseColor = "#ECBE06";

/////////////////////////////////////////////////////////////////////
/////////////////Controller configuration////////////////////////////
//////////////////////////////////////////////////////////////////////
        /*//0 for TV, 1 for DVD
        commands p = new commands();
        p.controller = 1;*/
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
                    sender.send(commands.FORWARD);
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
                    sender.send(commands.BACKWARD);
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
                    sender.send(commands.LEFT);
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
                    sender.send(commands.RIGHT);
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
////////////-----------SPEED UP------------------------/////////////////////////
        final Button SuButton = (Button) findViewById(R.id.speedup);

        SuButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "su";
                    sender.running = true;
                    sender.send(commands.SPEEDUP);
                    SuButton.setTextColor(Color.BLACK);
                    SuButton.setBackgroundColor(Color.parseColor(pressColor));
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("su")) {
                    sendingMotion = "0";
                    sender.running = false;
                    SuButton.setTextColor(Color.BLACK);
                    SuButton.setBackgroundColor(Color.parseColor(releaseColor));
                    return true;
                }
                return false;
            }
        });
////////////////--------------SPEED DOWN--------------------////////////////////////////
        final Button SoButton = (Button) findViewById(R.id.speeddown);

        SoButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "so";
                    sender.running = true;
                    sender.send(commands.SPEEDDOWN);
                    SoButton.setTextColor(Color.BLACK);
                    SoButton.setBackgroundColor(Color.parseColor(pressColor));
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("so")) {
                    sendingMotion = "0";
                    sender.running = false;
                    SoButton.setTextColor(Color.BLACK);
                    SoButton.setBackgroundColor(Color.parseColor(releaseColor));
                    return true;
                }
                return false;
            }
        });
////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////COLOR BUTTON SECTION//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////-----------------RED--------------------------------------------////////////////////
        final Button redButton = (Button) findViewById(R.id.red);

        redButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "red";
                    sender.running = true;
                    sender.send(commands.LIGHTS_RED); // sending LIGHTS_RED data to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("red")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
//////////////-------------------GREEN------------------------------//////////////////////////
        final Button greenButton = (Button) findViewById(R.id.green);

        greenButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "green";
                    sender.running = true;
                    sender.send(commands.LIGHTS_GREEN); // sending green data to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("green")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
//////////////////---------------------YELLOW----------------------------///////////////////
        final Button yellowButton = (Button) findViewById(R.id.yellow);

        yellowButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "yellow";
                    sender.running = true;
                    sender.send(commands.LIGHTS_YELLOW); // sending yellow to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("yellow")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
/////////////////-------------BLUE------------------------------------//////////////
        final Button blueButton = (Button) findViewById(R.id.blue);

        blueButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "blue";
                    sender.running = true;
                    sender.send(commands.LIGHTS_BLUE); // sending blue data to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("blue")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
//////////////-----------------------RAINBOW--------------------------///////////////////
        final Button rainbowButton = (Button) findViewById(R.id.rainbow);

        rainbowButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "rainbow";
                    sender.running = true;
                    sender.send(commands.LIGHTS_RAINBOW); // sending rainbow data to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("rainbow")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
//////////////////------------LIGHTS ON---------------------------------////////////////////
        final Button lightsOnButton = (Button) findViewById(R.id.lights_on);

        lightsOnButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "on";
                    sender.running = true;
                    sender.send(commands.LIGHTS_ON); // sending lights on data to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("on")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
///////////---------------Lights off-----------------------------//////////////////////
        final Button lightsOffButton = (Button) findViewById(R.id.lights_off);

        lightsOffButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "off";
                    sender.running = true;
                    sender.send(commands.LIGHTS_OFF); // sending lights off to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("off")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
///////////////////------------------LIGHTS AUTO----------------------------------////////////////
        final Button lightsAutoButton = (Button) findViewById(R.id.lights_auto);

        lightsAutoButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int action = event.getAction();

                if (action == MotionEvent.ACTION_DOWN && sendingMotion.equals("0")) {
                    sendingMotion = "lights_auto";
                    sender.running = true;
                    sender.send(commands.LIGHTS_AUTO); // sending lights auto to address defined at beginning
                    return true;

                } else if (action == MotionEvent.ACTION_UP && sendingMotion.equals("lights_auto")) {
                    sendingMotion = "0";
                    sender.running = false;
                    return true;
                }
                return false;
            }
        });
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////AUTO DRIVING SECTION////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    }

}
