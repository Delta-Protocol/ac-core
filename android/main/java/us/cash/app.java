package us.cash;

import android.app.Application;
import android.content.Context;
import android.util.Log;
import us.wallet.*;
import java.io.FileOutputStream;
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
                    w = new Wallet(getApplicationContext().getFilesDir()) {
                        @Override
                        public FileOutputStream getOutputStream(String filename) throws IOException {
                            Log.d("Wallet","getOutputStream "+filename);
                            return getApplicationContext().openFileOutput(filename, Context.MODE_PRIVATE);
                        }
                    };
                    if (ml!=null)
                    ml.on_wallet_init_success();
                }
                catch(IOException e) {
                    Log.d("App","ERROR "+e.getMessage());
                    w=null;
                    if (ml!=null) ml.on_wallet_init_error("Error: " + e.getMessage());
                }
            }
        });

        thread.start();


    }

    AppListener ml=null;


}
