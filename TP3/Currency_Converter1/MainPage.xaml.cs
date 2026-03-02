using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Currency_Converter1
{
    /// <summary>
    /// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        double TauxChange = 0;
        double Resultat;

        public MainPage()
        {
            this.InitializeComponent();
        }

        private void rbnSFR_Checked(object sender, RoutedEventArgs e)
        {
            if (rbnSFR.Checked)
            {
                TauxChange = 0.90665;

                PicDevise.Image = imageList1.Images[0];
            }
        }

        private void rbnUSD_Checked(object sender, RoutedEventArgs e)
        {
            if (rbnUSD.Checked)
            {
                TauxChange = 0.92;

                PicDevise.Image = imageList1.Images[1];
            }
        }

        private void rbnGBP_Checked(object sender, RoutedEventArgs e)
        {
            if (rbnGBP.Checked)
            {
                TauxChange = 1.17;

                PicDevise.Image = imageList1.Images[2];
            }
        }

        private void rbnYEN_Checked(object sender, RoutedEventArgs e)
        {
            if (rbnYEN.Checked)
            {
                TauxChange = 0.006;

                PicDevise.Image = imageList1.Images[3];
            }
        }

        private void rbnYUEN_Checked(object sender, RoutedEventArgs e)
        {
            if (rbnYUEN.Checked)
            {
                TauxChange = 0.12;

                PicDevise.Image = imageList1.Images[4];
            }
        }

        private void rbnAUD_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.59;

            PicDevise.Image = imageList1.Images[5];
        }

        private void btnConvertir_Click(object sender, RoutedEventArgs e)
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
