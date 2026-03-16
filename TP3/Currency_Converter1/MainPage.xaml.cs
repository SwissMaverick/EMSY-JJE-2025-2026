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
using Windows.UI.Xaml.Media.Imaging;

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Currency_Converter1
{
    /// <summary>
    /// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        double TauxChange = 0;

        public MainPage()
        {
            this.InitializeComponent();
        }

        private void rbnSFR_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.90665;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/Souisse.png");
            ImgCurrency.Source = bitmapImage;
        }

        private void rbnUSD_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.92;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/USA_F_Yeah.png");
            ImgCurrency.Source = bitmapImage;
        }

        private void rbnGBP_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 1.17;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/Bri_ish.png");
            ImgCurrency.Source = bitmapImage;
        }

        private void rbnYEN_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.006;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/Toyota_Banzai.png");
            ImgCurrency.Source = bitmapImage;
        }

        private void rbnYUEN_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.12;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/Xong_Xina.png");
            ImgCurrency.Source = bitmapImage;
        }

        private void rbnAUD_Checked(object sender, RoutedEventArgs e)
        {
            TauxChange = 0.59;

            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.UriSource = new Uri(ImgCurrency.BaseUri, "ms-appx:///Assets/Australia.png");
            ImgCurrency.Source = bitmapImage;
        }

        private async void btnConvertir_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (TauxChange == 0)
                {
                    var dialog = new Windows.UI.Popups.MessageDialog("Veuillez sélectionner une devise.");
                    await dialog.ShowAsync();
                    return;
                }

                double Montant = double.Parse(txtMontant.Text);

                double Resultat = Montant * TauxChange;

                txtResultat.Text = Resultat.ToString();
            }
            catch (Exception)
            {
                var dialog = new Windows.UI.Popups.MessageDialog("Erreur : Veuillez entrer un montant numérique valide (ex: 100)."); 
                await dialog.ShowAsync();
            }
        }
    }
}
