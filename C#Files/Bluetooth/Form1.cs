using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Blue
{
    public partial class Form1 : Form
    {
        static EventHandler<BluetoothWin32AuthenticationEventArgs> authHandler = new EventHandler<BluetoothWin32AuthenticationEventArgs>(handleAuthRequests);
        BluetoothWin32Authentication authenticator = new BluetoothWin32Authentication(authHandler);
        static private void handleAuthRequests(object sender, BluetoothWin32AuthenticationEventArgs e)
        {
            switch (e.AuthenticationMethod)
            {
                case BluetoothAuthenticationMethod.Legacy:
                    break;
                case BluetoothAuthenticationMethod.OutOfBand:
                    MessageBox.Show("Autentykacja poza pasmem");
                    break;
                case BluetoothAuthenticationMethod.NumericComparison:
                    if (e.JustWorksNumericComparison == true)
                    {
                        MessageBox.Show("Porównanie numerów");
                    }
                    else
                    {
                        if (MessageBox.Show(e.NumberOrPasskeyAsString, "Sprawdz parowane urządzenie", MessageBoxButtons.YesNo) == DialogResult.Yes)
                        {
                            e.Confirm = true;
                        }
                        else
                        {
                            e.Confirm = false;
                        }
                    }
                    break;

                case BluetoothAuthenticationMethod.PasskeyNotification:
                    MessageBox.Show("Notyfikacja o kluczu");
                    break;

                case BluetoothAuthenticationMethod.Passkey:
                    MessageBox.Show("Klucz");
                    break;

                default:
                    MessageBox.Show("Nieznany sposób obsługi parowania");
                    break;

            }
        }
        
        BluetoothDeviceInfo []bluetoothDevices = null;
        BluetoothDeviceInfo selected;
        BluetoothClient localClient;
        bool isPaired = false;
        string []selectedDevice = new string[2];

        public Form1()
        {
            InitializeComponent();
            localClient = new BluetoothClient();
            dataGridView1.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // disconnecting paired devices
            selected = null;
            if (isPaired)
            {
                BluetoothDeviceInfo[] paired = localClient.DiscoverDevices(255, false, true, false, false);
                BluetoothSecurity.RemoveDevice(paired[0].DeviceAddress);
                isPaired = false;
            }
            // clearing table of found devices
            foreach (DataGridViewRow row in dataGridView1.Rows) dataGridView1.Rows.Remove(row);
            if (BluetoothRadio.PrimaryRadio.Mode == RadioMode.PowerOff)
                BluetoothRadio.PrimaryRadio.Mode = RadioMode.Connectable;
            bluetoothDevices = localClient.DiscoverDevices(999);
            foreach (BluetoothDeviceInfo device in bluetoothDevices)
            {
                dataGridView1.Rows.Add(device.DeviceAddress.ToString(), device.DeviceName, device.Rssi.ToString());
                device.Update();
                device.Refresh();
            }
            if (dataGridView1.Rows.Count>0) selected = bluetoothDevices[0];
            if (dataGridView1.Rows.Count==0) dataGridView1.Rows.Add("<NULL>","Nie znaleziono urządzeń!", "NaN");
            selectedDevice[0] = dataGridView1.Rows[0].Cells[0].Value.ToString();
            selectedDevice[1] = dataGridView1.Rows[0].Cells[1].Value.ToString();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(dataGridView1.Rows.Count==0)
            {
                MessageBox.Show("Nie skanowano w poszukiwaniu urządzeń!", "BŁĄD!");
                return;
            }
            if (selectedDevice[0] == "<NULL>")
            {
                MessageBox.Show("Brak urzadzeń do sparowania!", "BŁĄD!");
                return;
            }
            if (isPaired)
            {
                BluetoothDeviceInfo[] paired = localClient.DiscoverDevices(255, false, true, false, false);
                if (paired[0].DeviceName == selectedDevice[1]) return;
                BluetoothSecurity.RemoveDevice(paired[0].DeviceAddress);
            }
            selected.Update();
            selected.Refresh();
            selected.SetServiceState(BluetoothService.ObexObjectPush, true);
            //localClient.Connect(new BluetoothEndPoint(selected.DeviceAddress, BluetoothService.ObexFileTransfer));
            BluetoothSecurity.PairRequest(selected.DeviceAddress, null);
            isPaired = true;
        }

        private void dataGridView1_SelectionChanged(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in dataGridView1.SelectedRows)
            {
                selectedDevice[0] = row.Cells[0].Value.ToString();
                selectedDevice[1] = row.Cells[1].Value.ToString();
            }
            foreach (BluetoothDeviceInfo device in bluetoothDevices)
            {
                if (selectedDevice[0] == device.DeviceAddress.ToString() && selectedDevice[1] == device.DeviceName)
                    selected = device;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            if (textBox1.Text == "") openFileDialog1.InitialDirectory = "c:\\Users\\Slize\\Desktop\\SemestrV\\UP\\lab";
            else openFileDialog1.InitialDirectory = textBox1.Text;
            openFileDialog1.Filter = "Pliki obrazu (*.jpg, *.png)|*.jpg;*.png";
            openFileDialog1.FilterIndex = 0;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string selectedFileName = openFileDialog1.FileName;
                listBox1.Items.Add(selectedFileName);
                textBox1.Text = selectedFileName;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if(isPaired==false)
            {
                MessageBox.Show("Trzeba najpierw sparować urządzenie!", "BŁĄD!");
                return;
            }
            listBox2.Items.Clear();
            progressBar1.Minimum = 1;
            progressBar1.Maximum = listBox1.Items.Count + 1;
            progressBar1.Value = 1;
            progressBar1.Step = 1;
            foreach (string path in listBox1.Items)
            {
                listBox2.Items.Add("Wysylanie pliku " + Path.GetFileName(path));
                var file = @path;
                var uri = new Uri("obex://" + selected.DeviceAddress + "/" + file);
                var request = new ObexWebRequest(uri);
                request.ReadFile(file);
                var response = (ObexWebResponse)request.GetResponse();
                listBox2.Items.Add(response.StatusCode.ToString());
                if (response.StatusCode.ToString()=="OK, Final") progressBar1.PerformStep();
                response.Close();
            }
            listBox1.Items.Clear();
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex < 0) return;
            DialogResult result = MessageBox.Show("Czy chcesz usunąć wybrany plik?", "Potwierdzenie akcji", MessageBoxButtons.YesNo);
            if(result==DialogResult.Yes)
            {
                listBox1.Items.RemoveAt(listBox1.SelectedIndex);
            }
        }
    }
}
