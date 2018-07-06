package com.example.usgov;

import android.app.Application;
import android.util.Log;
import us.wallet.*;

import java.io.IOException;


public class app extends Application {

    public Wallet w=null;

    public app() {
        initWallet();
    }

    void initWallet() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    w = new Wallet(getApplicationContext().getFilesDir());
                    if (ml!=null)
                    ml.on_wallet_init_success();
                }
                catch(IOException e) {
                    w=null;
                    if (ml!=null) ml.on_wallet_init_error(e.getMessage());
                }
            }
        });

        thread.start();


    }

    AppListener ml=null;


}
