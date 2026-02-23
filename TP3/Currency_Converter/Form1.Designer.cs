namespace Currency_Converter
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.PicDevise = new System.Windows.Forms.PictureBox();
            this.rbnSFR = new System.Windows.Forms.RadioButton();
            this.rbnUSD = new System.Windows.Forms.RadioButton();
            this.rbnGBP = new System.Windows.Forms.RadioButton();
            this.rbnYENJAP = new System.Windows.Forms.RadioButton();
            this.btnConvertir = new System.Windows.Forms.Button();
            this.txtMontant = new System.Windows.Forms.TextBox();
            this.txtResultat = new System.Windows.Forms.TextBox();
            this.labelResultEuro = new System.Windows.Forms.Label();
            this.labelMontant = new System.Windows.Forms.Label();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.rbnYUENCHIN = new System.Windows.Forms.RadioButton();
            this.rbnAUD = new System.Windows.Forms.RadioButton();
            ((System.ComponentModel.ISupportInitialize)(this.PicDevise)).BeginInit();
            this.SuspendLayout();
            // 
            // PicDevise
            // 
            this.PicDevise.Image = global::Ex1_Currency_Converter.Properties.Resources.Souisse;
            this.PicDevise.Location = new System.Drawing.Point(389, 26);
            this.PicDevise.Name = "PicDevise";
            this.PicDevise.Size = new System.Drawing.Size(309, 209);
            this.PicDevise.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.PicDevise.TabIndex = 0;
            this.PicDevise.TabStop = false;
            // 
            // rbnSFR
            // 
            this.rbnSFR.AutoSize = true;
            this.rbnSFR.Location = new System.Drawing.Point(63, 58);
            this.rbnSFR.Name = "rbnSFR";
            this.rbnSFR.Size = new System.Drawing.Size(89, 19);
            this.rbnSFR.TabIndex = 1;
            this.rbnSFR.TabStop = true;
            this.rbnSFR.Text = "Franc Suisse";
            this.rbnSFR.UseVisualStyleBackColor = true;
            this.rbnSFR.CheckedChanged += new System.EventHandler(this.rbnSFR_CheckedChanged_1);
            // 
            // rbnUSD
            // 
            this.rbnUSD.AutoSize = true;
            this.rbnUSD.Location = new System.Drawing.Point(63, 83);
            this.rbnUSD.Name = "rbnUSD";
            this.rbnUSD.Size = new System.Drawing.Size(77, 19);
            this.rbnUSD.TabIndex = 2;
            this.rbnUSD.TabStop = true;
            this.rbnUSD.Text = "US dollars";
            this.rbnUSD.UseVisualStyleBackColor = true;
            this.rbnUSD.CheckedChanged += new System.EventHandler(this.rbnUSD_CheckedChanged_1);
            // 
            // rbnGBP
            // 
            this.rbnGBP.AutoSize = true;
            this.rbnGBP.Location = new System.Drawing.Point(63, 108);
            this.rbnGBP.Name = "rbnGBP";
            this.rbnGBP.Size = new System.Drawing.Size(98, 19);
            this.rbnGBP.TabIndex = 3;
            this.rbnGBP.TabStop = true;
            this.rbnGBP.Text = "Livres Sterling";
            this.rbnGBP.UseVisualStyleBackColor = true;
            this.rbnGBP.CheckedChanged += new System.EventHandler(this.rbnGBP_CheckedChanged_1);
            // 
            // rbnYENJAP
            // 
            this.rbnYENJAP.AutoSize = true;
            this.rbnYENJAP.Location = new System.Drawing.Point(63, 133);
            this.rbnYENJAP.Name = "rbnYENJAP";
            this.rbnYENJAP.Size = new System.Drawing.Size(92, 19);
            this.rbnYENJAP.TabIndex = 4;
            this.rbnYENJAP.TabStop = true;
            this.rbnYENJAP.Text = "Yen Japonais";
            this.rbnYENJAP.UseVisualStyleBackColor = true;
            this.rbnYENJAP.CheckedChanged += new System.EventHandler(this.rbnYEN_CheckedChanged_1);
            // 
            // btnConvertir
            // 
            this.btnConvertir.BackColor = System.Drawing.SystemColors.Control;
            this.btnConvertir.Font = new System.Drawing.Font("Comic Sans MS", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.btnConvertir.Location = new System.Drawing.Point(304, 326);
            this.btnConvertir.Name = "btnConvertir";
            this.btnConvertir.Size = new System.Drawing.Size(152, 76);
            this.btnConvertir.TabIndex = 5;
            this.btnConvertir.Text = "Convertir";
            this.btnConvertir.UseVisualStyleBackColor = false;
            this.btnConvertir.Click += new System.EventHandler(this.btnConvertir_Click);
            // 
            // txtMontant
            // 
            this.txtMontant.Location = new System.Drawing.Point(97, 379);
            this.txtMontant.Name = "txtMontant";
            this.txtMontant.Size = new System.Drawing.Size(100, 23);
            this.txtMontant.TabIndex = 6;
            // 
            // txtResultat
            // 
            this.txtResultat.Location = new System.Drawing.Point(572, 379);
            this.txtResultat.Name = "txtResultat";
            this.txtResultat.Size = new System.Drawing.Size(100, 23);
            this.txtResultat.TabIndex = 7;
            // 
            // labelResultEuro
            // 
            this.labelResultEuro.AutoSize = true;
            this.labelResultEuro.Location = new System.Drawing.Point(580, 357);
            this.labelResultEuro.Name = "labelResultEuro";
            this.labelResultEuro.Size = new System.Drawing.Size(92, 15);
            this.labelResultEuro.TabIndex = 8;
            this.labelResultEuro.Text = "Resultat en Euro";
            // 
            // labelMontant
            // 
            this.labelMontant.AutoSize = true;
            this.labelMontant.Location = new System.Drawing.Point(97, 357);
            this.labelMontant.Name = "labelMontant";
            this.labelMontant.Size = new System.Drawing.Size(53, 15);
            this.labelMontant.TabIndex = 9;
            this.labelMontant.Text = "Montant";
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "Souisse.png");
            this.imageList1.Images.SetKeyName(1, "USA_F_Yeah.png");
            this.imageList1.Images.SetKeyName(2, "Bri_ish.png");
            this.imageList1.Images.SetKeyName(3, "Toyota_Banzai.png");
            this.imageList1.Images.SetKeyName(4, "Xong_Xina.png");
            this.imageList1.Images.SetKeyName(5, "Australia.png");
            // 
            // rbnYUENCHIN
            // 
            this.rbnYUENCHIN.AutoSize = true;
            this.rbnYUENCHIN.Location = new System.Drawing.Point(63, 158);
            this.rbnYUENCHIN.Name = "rbnYUENCHIN";
            this.rbnYUENCHIN.Size = new System.Drawing.Size(95, 19);
            this.rbnYUENCHIN.TabIndex = 10;
            this.rbnYUENCHIN.TabStop = true;
            this.rbnYUENCHIN.Text = "Yuen Chinois";
            this.rbnYUENCHIN.UseVisualStyleBackColor = true;
            this.rbnYUENCHIN.CheckedChanged += new System.EventHandler(this.rbnYENCHIN_CheckedChanged);
            // 
            // rbnAUD
            // 
            this.rbnAUD.AutoSize = true;
            this.rbnAUD.Location = new System.Drawing.Point(63, 183);
            this.rbnAUD.Name = "rbnAUD";
            this.rbnAUD.Size = new System.Drawing.Size(85, 19);
            this.rbnAUD.TabIndex = 11;
            this.rbnAUD.TabStop = true;
            this.rbnAUD.Text = "AUS dollars";
            this.rbnAUD.UseVisualStyleBackColor = true;
            this.rbnAUD.CheckedChanged += new System.EventHandler(this.rbnAUD_CheckedChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.rbnAUD);
            this.Controls.Add(this.rbnYUENCHIN);
            this.Controls.Add(this.labelMontant);
            this.Controls.Add(this.labelResultEuro);
            this.Controls.Add(this.txtResultat);
            this.Controls.Add(this.txtMontant);
            this.Controls.Add(this.btnConvertir);
            this.Controls.Add(this.rbnYENJAP);
            this.Controls.Add(this.rbnGBP);
            this.Controls.Add(this.rbnUSD);
            this.Controls.Add(this.rbnSFR);
            this.Controls.Add(this.PicDevise);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.PicDevise)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private PictureBox PicDevise;
        private RadioButton rbnSFR;
        private RadioButton rbnUSD;
        private RadioButton rbnGBP;
        private RadioButton rbnYENJAP;
        private Button btnConvertir;
        private TextBox txtMontant;
        private TextBox txtResultat;
        private Label labelResultEuro;
        private Label labelMontant;
        private ImageList imageList1;
        private RadioButton rbnYUENCHIN;
        private RadioButton rbnAUD;
    }
}