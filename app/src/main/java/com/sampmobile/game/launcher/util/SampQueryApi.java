package com.sampmobile.game.launcher.util;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;
import java.util.StringTokenizer;

public class SampQueryApi {
    private final String charset = "windows-1251";
    private final Random f = new Random();
    private boolean isValidAddr = true;
    private String serveraddress = "";
    private InetAddress serverip = null;
    private final int serverport;
    private DatagramSocket socket = null;

    public SampQueryApi(String str, int i) {
        try {
            InetAddress byName = InetAddress.getByName(str);
            this.serverip = byName;
            this.serveraddress = byName.getHostAddress();
        } catch (UnknownHostException unused) {
            this.isValidAddr = false;
        }
        try {
            DatagramSocket datagramSocket = new DatagramSocket();
            this.socket = datagramSocket;
            datagramSocket.setSoTimeout(2000);
        } catch (SocketException unused2) {
            this.isValidAddr = false;
        }
        this.serverport = i;
    }

    private DatagramPacket initPacket(String str) {
        try {
            byte[] bytes = ("SAMPzalupa" + str).getBytes("windows-1251");
            StringTokenizer stringTokenizer = new StringTokenizer(this.serveraddress, ".");
            bytes[4] = (byte) Integer.parseInt(stringTokenizer.nextToken());
            bytes[5] = (byte) Integer.parseInt(stringTokenizer.nextToken());
            bytes[6] = (byte) Integer.parseInt(stringTokenizer.nextToken());
            bytes[7] = (byte) Integer.parseInt(stringTokenizer.nextToken());
            int i = this.serverport;
            bytes[8] = (byte) (i & 255);
            bytes[9] = (byte) ((i >> 8) & 255);
            return new DatagramPacket(bytes, bytes.length, this.serverip, this.serverport);
        } catch (Exception unused) {
            return null;
        }
    }

    private byte[] receiveData() {
        DatagramPacket datagramPacket;
        if (this.socket == null) {
            return new byte[3072];
        }
        DatagramPacket datagramPacket2 = null;
        datagramPacket = new DatagramPacket(new byte[3072], 3072);
        try {
            this.socket.receive(datagramPacket);
        } catch (IOException unused) {
            datagramPacket2 = datagramPacket;
        }
        return datagramPacket.getData();
    }

    private void sendPacket(DatagramPacket datagramPacket) {
        try {
            DatagramSocket datagramSocket = this.socket;
            if (datagramSocket != null) {
                datagramSocket.send(datagramPacket);
            }
        } catch (IOException e) {
        }
    }

    public void close() {
        DatagramSocket datagramSocket = this.socket;
        if (datagramSocket != null) {
            datagramSocket.close();
        }
    }

    public boolean isOnline() {
        if (!this.isValidAddr || this.socket == null) {
            return false;
        }
        byte[] f2 = f();
        byte[] bArr = null;
        try {
            String str = new String(f2, "windows-1251");
            sendPacket(initPacket("p" + str));
            bArr = receiveData();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        if (bArr[10] == 112 && bArr[11] == f2[0] && bArr[12] == f2[1] && bArr[13] == f2[2] && bArr[14] == f2[3]) {
            return true;
        }
        return false;
    }

    public String[] getInfo() {
        String[] strArr = new String[6];
        try {
            sendPacket(initPacket("i"));
            ByteBuffer wrap = ByteBuffer.wrap(receiveData());
            wrap.order(ByteOrder.LITTLE_ENDIAN);
            wrap.position(11);
            if (wrap.get() == 0) {
                strArr[0] = "0";
            } else {
                strArr[0] = "1";
            }
            strArr[1] = String.valueOf((int) wrap.getShort());
            strArr[2] = String.valueOf((int) wrap.getShort());
            strArr[3] = convert(wrap, wrap.getInt());
            strArr[4] = convert(wrap, wrap.getInt());
            strArr[5] = convert(wrap, wrap.getInt());
            return strArr;
        } catch (Exception unused) {
            return null;
        }
    }

    public long e() {
        try {
            byte[] f2 = f();
            long currentTimeMillis = System.currentTimeMillis();
            sendPacket(initPacket("p" + new String(f2, "windows-1251")));
            receiveData();
            return System.currentTimeMillis() - currentTimeMillis;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return -1;
        }
    }

    /* access modifiers changed from: package-private */
    public byte[] f() {
        byte[] bArr = new byte[4];
        this.f.nextBytes(bArr);
        bArr[0] = (byte) (((bArr[0] % 100) + 110) & 255);
        bArr[1] = (byte) (bArr[1] % -128);
        bArr[2] = (byte) (bArr[2] % -128);
        bArr[3] = (byte) ((bArr[3] % 50) & 255);
        return bArr;
    }

    private String convert(ByteBuffer byteBuffer, int i) throws UnsupportedEncodingException {
        byte[] bArr = new byte[i];
        for (int i2 = 0; i2 < i; i2++) {
            try {
                bArr[i2] = byteBuffer.get();
            } catch (Exception unused) {
            }
        }
        return new String(bArr, "windows-1251");
    }
}

