package us.wallet;

import java.security.SecureRandom;
import java.security.Security;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import org.spongycastle.util.Arrays;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.jce.provider.BouncyCastleProvider;
import javax.crypto.spec.SecretKeySpec;
import java.security.PrivateKey;
import java.security.PublicKey;

public class SymmetricEncryption {

    private static final SecureRandom random = new SecureRandom();
    private static final int iv_size = 12;
    private Cipher cipher = null;
    private byte[] key = null;
    private byte[] iv = null;
    SecretKeySpec keySpec = null;

    public SymmetricEncryption(PrivateKey privA, PublicKey pubB) throws Exception {
        Security.insertProviderAt(new BouncyCastleProvider(), 1);
        cipher = Cipher.getInstance("AES/GCM/NoPadding", "BC");
        iv = new byte[iv_size];
        key = EllipticCryptography.getInstance().generateSharedKey(privA,pubB);
    }

    public byte[] encrypt(byte[] plaintext) throws Exception {
        random.nextBytes(iv);
        cipher.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
        return Arrays.concatenate(cipher.doFinal(plaintext), iv);
    }

    public byte[] decrypt(byte[] encrypted) throws Exception {
        iv = Arrays.copyOfRange(encrypted, encrypted.length - iv_size, encrypted.length);
        cipher.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
        return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, encrypted.length - iv_size));
    }

}
