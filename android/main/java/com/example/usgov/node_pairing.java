package com.example.usgov;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.IOException;

public class node_pairing extends AppCompatActivity {
    private EditText addr;
    private Button pair;
    private Button test;


    void test_result(final String balance, final String node_addr) {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                final app a=(app)getApplication();
                try {
                    a.w.set_walletd_host(node_addr);
                    if (balance == "?") {
                        Toast.makeText(getApplicationContext(), "Nice try, but wrong", 6000).show();
                    } else {
                        Toast.makeText(node_pairing.this, "walletd is alive at " + a.w.walletd_host(), 30000).show();
                    }
                }
                catch (IOException e) {
                    Toast.makeText(getApplicationContext(), "Unexpected error", 6000).show();
                }

            }
        }));

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_node_pairing);
//        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);
/*
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });


*/

        addr = (EditText) findViewById(R.id.walletd_address);
        pair = (Button) findViewById(R.id.pair);

        test = (Button) findViewById(R.id.test);

        addr.setText(((app)getApplication()).w.walletd_host());


        test.setOnClickListener(new View.OnClickListener() {
                                    @Override
                                    public void onClick(View view) {

        Thread thread = new Thread(new Runnable() {
            final String address=addr.getText().toString();
            final app a=(app)getApplication();
            final String h=a.w.walletd_host();
            @Override
            public void run() {
                try {
                    a.w.set_walletd_host(address);
                    String b=a.w.balance(false);
                    test_result(b,address);
                }
                catch (IOException e) {
                    test_result("?",address);
                }
            }
        });

        thread.start();


                                           /*
           if (w==null) {
               Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
               return;
           }
           if (balance.getText() == balance_button_text) {
               balance.setText("...");
               //final Handler handler = new Handler();
               Thread thread = new Thread(new Runnable() {
                   @Override
                   public void run() {
                       final String b = w.balance(false);
                       setBalance(b);
                           / *
                           Log.d("ZZZZZZZZZZZZ",b);
                           handler.post(new Runnable() {
                           handler.post(new Runnable() {
                               @Override
                               public void run() {

                               }
                           });
                           * /
                           }
                       });

                       thread.start();

                   } else {
                       balance.setText(balance_button_text);
                   }
    */
                                    }
                                }

        );

       pair.setOnClickListener(new View.OnClickListener() {
                                       @Override
                                       public void onClick(View view) {

       app a=(app)getApplication();
       try {
           a.w.set_walletd_host(addr.getText().toString());
           finish();
       }
       catch (IOException e) {
           Toast.makeText(getApplicationContext(), "Nice try, but wrong", 6000).show();
       }

                                           /*
           if (w==null) {
               Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
               return;
           }
           if (balance.getText() == balance_button_text) {
               balance.setText("...");
               //final Handler handler = new Handler();
               Thread thread = new Thread(new Runnable() {
                   @Override
                   public void run() {
                       final String b = w.balance(false);
                       setBalance(b);
                           / *
                           Log.d("ZZZZZZZZZZZZ",b);
                           handler.post(new Runnable() {
                           handler.post(new Runnable() {
                               @Override
                               public void run() {

                               }
                           });
                           * /
                           }
                       });

                       thread.start();

                   } else {
                       balance.setText(balance_button_text);
                   }
    */
               }
           }

        );


    }

}
