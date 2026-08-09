package com.example.androidnode;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.Gravity;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;


import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;


public class MainActivity extends Activity {

    private TextView terminal;
    private EditText commandInput;

    private static boolean nodeStarted = false;

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("node");
    }

    private native int startNode();
    private native int sendCommand(String command);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        createTerminalUI();

        if (!nodeStarted) {
            nodeStarted = true;

            new Thread(() -> {
                int result = startNode();

                appendTerminal(
                        "\nNode exited with code: " + result + "\n"
                );
            }).start();
        }
    }

    private void createTerminalUI() {

        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.BLACK);

        TextView title = new TextView(this);
        title.setText("  Android Node Terminal");
        title.setTextColor(Color.WHITE);
        title.setTextSize(18);
        title.setGravity(Gravity.CENTER_VERTICAL);

        root.addView(
                title,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        60
                )
        );

        terminal = new TextView(this);
        terminal.setTextColor(Color.WHITE);
        terminal.setTextSize(14);
        terminal.setPadding(16, 16, 16, 16);
        terminal.setMovementMethod(
                ScrollingMovementMethod.getInstance()
        );

        terminal.setText(
                "Embedded Node.js Terminal\n" +
                "Starting Node.js...\n\n"
        );

        root.addView(
                terminal,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        0,
                        1
                )
        );

        LinearLayout bottom = new LinearLayout(this);
        bottom.setOrientation(LinearLayout.HORIZONTAL);
        bottom.setPadding(8, 8, 8, 8);

        commandInput = new EditText(this);
        commandInput.setTextColor(Color.WHITE);
        commandInput.setHintTextColor(Color.GRAY);
        commandInput.setHint("Enter Node command...");
        commandInput.setSingleLine(true);
        commandInput.setImeOptions(
                EditorInfo.IME_ACTION_SEND
        );

        bottom.addView(
                commandInput,
                new LinearLayout.LayoutParams(
                        0,
                        LinearLayout.LayoutParams.WRAP_CONTENT,
                        1
                )
        );

        Button send = new Button(this);
        send.setText("Run");

        bottom.addView(
                send,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.WRAP_CONTENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT
                )
        );

        View.OnClickListener listener = v -> executeCommand();

        send.setOnClickListener(listener);

        commandInput.setOnEditorActionListener(
                (v, actionId, event) -> {
                    executeCommand();
                    return true;
                }
        );

        root.addView(
                bottom,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT
                )
        );

        setContentView(root);
    }

    private void executeCommand() {

        String command = commandInput.getText().toString();

        if (command.trim().isEmpty()) {
            return;
        }

        appendTerminal("$ " + command + "\n");

        commandInput.setText("");

        new Thread(() -> {

            int result = sendCommand(command);

            if (result != 0) {
                appendTerminal(
                        "[send error: " + result + "]\n"
                );
            }

        }).start();
    }

    public void appendTerminal(String text) {

        runOnUiThread(() -> {

            terminal.append(text);

            terminal.post(() ->
                    terminal.scrollTo(
                            0,
                            terminal.getBottom()
                    )
            );
        });
    }
}
