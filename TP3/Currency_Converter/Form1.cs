namespace Currency_Converter
{
    public partial class Form1 : Form
    {
        double TauxChange = 0;
        double Resultat;

        public Form1()
        {
            InitializeComponent();
        }

        private void rbnSFR_CheckedChanged_1(object sender, EventArgs e)
        {
            if (rbnSFR.Checked)
            {
                TauxChange = 0.90665;

                PicDevise.Image = imageList1.Images[0];
            }
        }

        private void rbnUSD_CheckedChanged_1(object sender, EventArgs e)
        {
            if (rbnUSD.Checked)
            {
                TauxChange = 0.92;

                PicDevise.Image = imageList1.Images[1];
            }
        }

        private void rbnGBP_CheckedChanged_1(object sender, EventArgs e)
        {
            if (rbnGBP.Checked)
            {
                TauxChange = 1.17;

                PicDevise.Image = imageList1.Images[2];
            }
        }

        private void rbnYEN_CheckedChanged_1(object sender, EventArgs e)
        {
            if (rbnYENJAP.Checked)
            {
                TauxChange = 0.006;

                PicDevise.Image = imageList1.Images[3];
            }
        }

        private void rbnYENCHIN_CheckedChanged(object sender, EventArgs e)
        {
            if (rbnYUENCHIN.Checked)
            {
                TauxChange = 0.12;

                PicDevise.Image = imageList1.Images[4];
            }
        }

        private void rbnAUD_CheckedChanged(object sender, EventArgs e)
        {
            TauxChange = 0.59;

            PicDevise.Image = imageList1.Images[5];
        }

        private void btnConvertir_Click(object sender, EventArgs e)
        {
            try
            {
                if (TauxChange == 0)
                {
                    MessageBox.Show("Veuillez sélectionner une devise.");
                    return;
                }

                double Montant = double.Parse(txtMontant.Text);

                double Resultat = Montant * TauxChange;

                txtResultat.Text = Resultat.ToString();
            }
            catch (Exception)
            {
                MessageBox.Show("Erreur : Veuillez entrer un montant numérique valide (ex: 100).");
            }
        }
    }
}