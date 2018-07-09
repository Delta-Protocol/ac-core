package us.cash;

import android.app.Activity;
import android.content.Context;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.tech.IsoDep;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.util.Arrays;

public class SmartCardReader implements NfcAdapter.ReaderCallback { // CreateNdefMessageCallback, OnNdefPushCompleteCallback {
    MainActivity a;
    NfcAdapter mNfcAdapter;
    boolean reading=false;


    boolean is_reading() {
        return reading;
    }

    public SmartCardReader(MainActivity a_) {
        a=a_;
        mNfcAdapter = NfcAdapter.getDefaultAdapter(a);
        if (mNfcAdapter == null) {
//            Toast.makeText(a, "NFC is not available", Toast.LENGTH_LONG).show();
//                finish();
            return;
        }
        // Register callback
        //mNfcAdapter.setNdefPushMessageCallback(this, a);
    }

    boolean nfc_available() {
        return mNfcAdapter != null;
    }

        /*
        void processIntent(Intent intent) {
            Parcelable[] rawMsgs = intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
            // only one message sent during the beam
            NdefMessage msg = (NdefMessage) rawMsgs[0];
            // record 0 contains the MIME type, record 1 is the AAR, if present
            Toast.makeText(a, new String(msg.getRecords()[0].getPayload()), Toast.LENGTH_LONG).show();

        }
*/
/*
https://medium.com/the-almanac/how-to-build-a-simple-smart-card-emulator-reader-for-android-7975fae4040f

HEADER:00-A4-04-00
CLA: the Class byte is used to indicate to what extent the command complies with the ISO7816, and if so, what kind of “Secure Messaging” will be used. To keep things simple, we will not be using this byte in our example and we will be passing `00` all the time.
INS: the Instruction byte is used to indicate what method we want to execute, this can be a variety of methods like: `A4` to Select a File or Applet, `B0` to Read a Binary, `D0` to Write a Binary … (see full list of Instructions http://www.cardwerk.com/smartcards/smartcard_standard_ISO7816-4_5_basic_organizations.aspx#table11)
P1 & P2: these two Parameter bytes are used for further customization of the Instruction, they depend on what custom commands the card specifies. Click here for the list of the possible cases

 */

    byte[] APDUCommand() {
        int sz=4+1+HostCardEmulatorService.AID.length+1;
        byte[] r=new byte[sz];
        r[0]=HostCardEmulatorService.CLA;
        r[1]=HostCardEmulatorService.INS;
        r[2]=(byte)0x04;
        r[3]=(byte)0x00;
        r[4]=(byte)(HostCardEmulatorService.AID.length);
        for (int i=0; i<HostCardEmulatorService.AID.length; ++i) {
            r[5+i]=HostCardEmulatorService.AID[i];
        }
        r[sz-1]=HostCardEmulatorService.LE;
        return r;
    }




    @Override
    public void onTagDiscovered(Tag tag) {
        Log.d("us.cash", "onTagDiscovered "+tag.toString());
        //1-2673/us.cash D/ onTagDiscovered TAG: Tech [android.nfc.tech.IsoDep, android.nfc.tech.NfcA]

        //String addr="23TeQWLmU5MUruhZc4SQuXhv6kHa";
        String addr="";
        Log.d("us.cash", "APDUCommand "+HostCardEmulatorService.ByteArrayToHexString(APDUCommand()));
        try {
            IsoDep isoDep = IsoDep.get(tag);
            isoDep.connect();



            //00A4040007A0000002471001
            //isoDep.getMaxTransceiveLength()
            byte[] response = isoDep.transceive(APDUCommand());
            if (response.length>2) {
                Log.d("SmartCardReader", String.valueOf(response.length));
                Log.d("SmartCardReader", String.valueOf(response[response.length-2]));
                Log.d("SmartCardReader", String.valueOf(response[response.length-1]));
                if (response[response.length-2]==HostCardEmulatorService.STATUS_SUCCESS[0] && response[response.length-1]==HostCardEmulatorService.STATUS_SUCCESS[1]) {
                    Log.d("SmartCardReader","B ");
                    response = Arrays.copyOf(response, response.length-2);
                    final String r=new String(response, "UTF-8");
                    Log.d("SmartCardReader",r);
                    a.runOnUiThread(new Thread(new Runnable() {
                        public void run() {
                            a.deliver_address_from_NFC(r);
                        }
                    }));
                }
            }
            Log.d("SmartCardReader","CardResponse: " + HostCardEmulatorService.ByteArrayToHexString(response));
            /*
            */
            isoDep.close();
        }
        catch(IOException e) {
            Log.d("us.cash", "onTagDiscovered "+tag.toString()+ "crashed "+e.getMessage());
        }
    }

    /*
            public NdefRecord createMimeRecord(String payload) {
                NdefRecord mimeRecord = new NdefRecord(
                        NdefRecord.TNF_MIME_MEDIA ,
                        "application/us.cash".getBytes(Charset.forName("US-ASCII")),
                        new byte[0], payload.getBytes(Charset.forName("US-ASCII")));
                return mimeRecord;
            }
            public NdefRecord createAR() {
                return NdefRecord.createApplicationRecord("us.cash");
            }

            @Override
            public NdefMessage createNdefMessage(NfcEvent event) {
                Toast.makeText(a, "nfc_cb callback createNdefMessage", Toast.LENGTH_LONG).show();

                NdefMessage msg = new NdefMessage(
                        new NdefRecord[]{
                                createMimeRecord("A abcdefabcdef"),
                                createAR()
                        });
                return msg;
            }

    */
    public void disable() {
        if (!nfc_available()) return;
        if (reading) {
            Log.d("ZZZ","stop reading");
            mNfcAdapter.disableReaderMode(a);
            reading = false;
        }
    }


    public void enable() {
        if (!nfc_available()) return;
        if (!reading) {
            Log.d("ZZZ","start reading");
            mNfcAdapter.enableReaderMode(a, this, NfcAdapter.FLAG_READER_NFC_A | NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK, null);
            reading = true;
        }
    }
/*
        @Override
        public void onNdefPushComplete(NfcEvent event) {
            Toast.makeText(a, "onNdefPushComplete", Toast.LENGTH_LONG).show();
        }
        */
}
