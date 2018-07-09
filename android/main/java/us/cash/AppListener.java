package us.cash;

public interface AppListener {
    public void on_wallet_init_success();
    public void on_wallet_init_error(String error);
}
