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

        private SerialDevice serialPort = null;
        DataWriter dataWriteObject = null;
        DataReader dataReaderObject = null;

        private ObservableCollection<DeviceInformation> listOfDevices;
        private CancellationTokenSource ReadCancellationTokenSource;

        public MainPage()
        {
            this.InitializeComponent();
            comPortInput.IsEnabled = false;
            sendTextButton.IsEnabled = false;
            listOfDevices = new ObservableCollection<DeviceInformation>();
            ListAvailablePorts();
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

        private async void ListAvailablePorts()
        {
            try
            {
                string aqs = SerialDevice.GetDeviceSelector();
                var dis = await DeviceInformation.FindAllAsync(aqs);

                status.Text = "Select a device and connect";

                for (int i = 0; i < dis.Count; i++)
                {
                    listOfDevices.Add(dis[i]);
                }

                DeviceListSource.Source = listOfDevices;
                comPortInput.IsEnabled = true;
                ConnectDevices.SelectedIndex = -1;
            }
            catch (Exception ex)
            {
                status.Text = ex.Message;
            }
        }

        private void sendTextButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (serialPort != null)
                {
                    // Create the DataWriter object and attach to OutputStream
                    dataWriteObject = new DataWriter(serialPort.OutputStream);

                    //Launch the WriteAsync task to perform the write
                    await WriteAsync();
                }
                else
                {
                    status.Text = "Select a device and connect";
                }
            }
            catch (Exception ex)
            {
                status.Text = "sendTextButton_Click: " + ex.Message;
            }
            finally
            {
                // Cleanup once complete
                if (dataWriteObject != null)
                {
                    dataWriteObject.DetachStream();
                    dataWriteObject = null;
                }
            }
        }

        private void comPortInput_Click(object sender, RoutedEventArgs e)
        {
            var selection = ConnectDevices.SelectedItems;

            if (selection.Count <= 0)
            {
                status.Text = "Select a device and connect";
                return;
            }

            DeviceInformation entry = (DeviceInformation)selection[0];

            try
            {
                serialPort = await SerialDevice.FromIdAsync(entry.Id);
                if (serialPort == null) return;

                // Disable the 'Connect' button 
                comPortInput.IsEnabled = false;

                // Configure serial settings
                serialPort.WriteTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.ReadTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.BaudRate = 9600;
                serialPort.Parity = SerialParity.None;
                serialPort.StopBits = SerialStopBitCount.One;
                serialPort.DataBits = 8;
                serialPort.Handshake = SerialHandshake.None;

                // Display configured settings
                status.Text = "Serial port configured successfully: ";
                status.Text += serialPort.BaudRate + "-";
                status.Text += serialPort.DataBits + "-";
                status.Text += serialPort.Parity.ToString() + "-";
                status.Text += serialPort.StopBits;

                // Set the RcvdText field to invoke the TextChanged callback
                // The callback launches an async Read task to wait for data
                rcvdText.Text = "Waiting for data...";

                // Create cancellation token object to close I/O operations when closing the device
                ReadCancellationTokenSource = new CancellationTokenSource();

                // Enable 'WRITE' button to allow sending data
                sendTextButton.IsEnabled = true;

                Listen();
            }
            catch (Exception ex)
            {
                status.Text = ex.Message;
                comPortInput.IsEnabled = true;
                sendTextButton.IsEnabled = false;
            }
        }

        private async Task WriteAsync()
        {
            Task<UInt32> storeAsyncTask;

            if (sendText.Text.Length != 0)
            {
                // Load the text from the sendText input text box to the dataWriter object
                dataWriteObject.WriteString(sendText.Text);

                // Launch an async task to complete the write operation
                storeAsyncTask = dataWriteObject.StoreAsync().AsTask();

                UInt32 bytesWritten = await storeAsyncTask;
                if (bytesWritten > 0)
                {
                    status.Text = sendText.Text + ", ";
                    status.Text += "bytes written successfully!";
                }
                sendText.Text = "";
            }
            else
            {
                status.Text = "Enter the text you want to write and then click on 'WRITE'";
            }
        }

        private async void Listen()
        {
            try
            {
                if (serialPort != null)
                {
                    dataReaderObject = new DataReader(serialPort.InputStream);

                    // keep reading the serial input
                    while (true)
                    {
                        await ReadAsync(ReadCancellationTokenSource.Token);
                    }
                }
            }
            catch (TaskCanceledException tce)
            {
                status.Text = "Reading task was cancelled, closing device and cleaning up";
                CloseDevice();
            }
            catch (Exception ex)
            {
                status.Text = ex.Message;
            }
            finally
            {
                // Cleanup once complete
                if (dataReaderObject != null)
                {
                    dataReaderObject.DetachStream();
                    dataReaderObject = null;
                }
            }
        }

        private async Task ReadAsync(CancellationToken cancellationToken)
        {
            Task<UInt32> loadAsyncTask;

            uint ReadBufferLength = 1024;

            // If task cancellation was requested, comply
            cancellationToken.ThrowIfCancellationRequested();

            // Set InputStreamOptions to complete the asynchronous read operation when one or more bytes is available
            dataReaderObject.InputStreamOptions = InputStreamOptions.Partial;

            using (var childCancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken))
            {
                // Create a task object to wait for data on the serialPort.InputStream
                loadAsyncTask = dataReaderObject.LoadAsync(ReadBufferLength).AsTask(childCancellationTokenSource.Token);

                // Launch the task and wait
                UInt32 bytesRead = await loadAsyncTask;
                if (bytesRead > 0)
                {
                    rcvdText.Text = dataReaderObject.ReadString(bytesRead);
                    status.Text = "bytes read successfully!";
                }
            }
        }

        private void CancelReadTask()
        {
            if (ReadCancellationTokenSource != null)
            {
                if (!ReadCancellationTokenSource.IsCancellationRequested)
                {
                    ReadCancellationTokenSource.Cancel();
                }
            }
        }

        private void CloseDevice()
        {
            if (serialPort != null)
            {
                serialPort.Dispose();
            }
            serialPort = null;

            comPortInput.IsEnabled = true;
            sendTextButton.IsEnabled = false;
            rcvdText.Text = "";
            listOfDevices.Clear();
        }

        private void closeDevice_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                status.Text = "";
                CancelReadTask();
                CloseDevice();
                ListAvailablePorts();
            }
            catch (Exception ex)
            {
                status.Text = ex.Message;
            }
        }
    }
}
