package com.example.androidnode;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {

    private static boolean nodeStarted = false;

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("node");
    }

    public native int startNodeWithArguments(String[] arguments);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        TextView text = new TextView(this);

        text.setText(
                "Android Node Runtime\n\n" +
                "Starting Node.js..."
        );

        text.setTextSize(20);
        text.setPadding(40, 40, 40, 40);

        setContentView(text);

        if (!nodeStarted) {
            nodeStarted = true;

            new Thread(() -> {

                startNodeWithArguments(
                        new String[] {
                                "node",
                                "-e",
                                "require('fs');" +
                                "console.log('Hello from embedded Node.js!');" +
                                "console.log(process.version);" +
                                "console.log(process.platform);" +
                                "console.log(process.arch);" +
                                "setInterval(() => console.log('Node alive'), 5000);"
                        }
                );

            }).start();
        }
    }
}
