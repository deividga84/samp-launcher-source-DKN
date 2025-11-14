package com.sampmobile.game.launcher.adapters;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.util.ButtonAnimator;

import org.w3c.dom.Text;

import java.util.ArrayList;

public class FaqAdapter extends RecyclerView.Adapter<FaqAdapter.ViewHolder> {

    private Activity mActivity;

    private ArrayList<String> mItemsMain = new ArrayList<>();
    private ArrayList<String> mItemsInfo = new ArrayList<>();

    private AlertDialog.Builder builder;

    public FaqAdapter(Activity activity)
    {
        mActivity = activity;

        mItemsMain.add("O servidor não respondeu. Tente novamente");
        mItemsMain.add("Você está banido deste servidor");
        mItemsMain.add("O servidor enviou as linhas em azul");
        mItemsMain.add("Janela de login, sem meu registro");
        mItemsMain.add("Não concordo com a decisão do administrador");

        mItemsInfo.add("Tente reiniciar o inicializador e faça login novamente. E se não ajudar, escreva para nós na seção técnica");
        mItemsInfo.add("Entre novamente no jogo, se não ajudar, desligue o WI-FI, tente fazer login na Internet móvel. Se ambos os métodos não ajudarem, escreva-nos no discord");
        mItemsInfo.add("Isso significa que seu apelido não atende aos requisitos do SAMP. O apelido deve ter de 6 a 21 caracteres e ter '_'.");
        mItemsInfo.add("Isso significa que seu apelido já está em uso. Altere no inicializador e tente novamente");
        mItemsInfo.add("Escreva-nos uma reclamação no discord e iremos considerá-la dentro de 48 horas!");

        builder = new AlertDialog.Builder(mActivity);
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.faq_item, parent, false);

        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        if(mItemsMain.size() > position)
        {
            holder.mFaqText.setText(mItemsMain.get(position));

            holder.mMain.setOnTouchListener(new ButtonAnimator(mActivity, holder.mMain));
            holder.mMain.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    builder.setMessage(mItemsInfo.get(holder.getAdapterPosition()))
                            .setCancelable(false)
                            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    dialog.dismiss();
                                }
                            })
                            .setNegativeButton("", null);
                    //Creating dialog box
                    AlertDialog alert = builder.create();
                    //Setting the title manually
                    alert.setTitle(mItemsMain.get(holder.getAdapterPosition()));
                    alert.show();
                }
            });
        }
    }

    @Override
    public int getItemCount() {
        return mItemsMain.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder
    {
        private View mView;
        public ImageView mMain;
        public TextView mFaqText;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            mMain = view.findViewById(R.id.faq_main);
            mFaqText = view.findViewById(R.id.faq_text);
        }

        public View getView() {
            return mView;
        }
    }
}
