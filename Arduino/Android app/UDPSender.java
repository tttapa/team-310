package testproject.test;

import android.os.AsyncTask;
import android.os.NetworkOnMainThreadException;
import android.provider.ContactsContract;

import android.os.StrictMode;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.io.*;
import java.util.Date;


/**
 * Created by Dirk Vanbeveren on 31/03/2017.
 */

class UDPSender {
    int port;
    String host;
    boolean running;
    int sendingInterval;
    private int data;
    void send(final int dat) {
        //inserting data in class
        this.data = dat;

        //repetition module on new thread
        final Thread sendingThread = new Thread() {
            public void run() {

                try {
                    while (running) {
                        //Strictmode to delete Network error
                        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
                        StrictMode.setThreadPolicy(policy);

                        Connection con = new Connection();
                        con.doInBackground();

                        //System.out.println(new Date());
                        Thread.sleep(sendingInterval);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

            }
        };
        sendingThread.start();

    }

    //class to avoid network error
    private class Connection extends AsyncTask {
        @Override
        protected Object doInBackground(Object... arg0) {
            packetSend();
            return null;
        }
    }


    private void packetSend() {
        try {
            //String host = "www.wifi.com";
            //int port = 90;


            byte[] message = {(byte)data};

            System.out.print("/////////////////" + "\n" + "data: " + data + "\n" + "byte message: ");
            for(int i = 0; i< message.length; i++) {
                System.out.print(message[i]+" ");
            }
            System.out.println();
            //System.out.println("///////packet sender");

            //internet address
            InetAddress address = InetAddress.getByName(host);

            //System.out.println("check2");

            //initialize datagram packet with data and message
            DatagramPacket packet = new DatagramPacket(message, 1, address, port);
            //Creating a socket --> sending packages --> closing socket.
            DatagramSocket dsocket = new DatagramSocket();
            dsocket.setBroadcast(false);
            dsocket.send(packet);
            dsocket.close();
        } catch (Exception e) {
            System.err.println(e);
            this.running = false;
        }
    }
}
