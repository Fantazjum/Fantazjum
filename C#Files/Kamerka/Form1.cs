using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AForge.Imaging;
using AForge.Video;
using AForge.Video.DirectShow;

namespace Kamerka
{
    public partial class Form1 : Form
    {
        private FilterInfoCollection listaVideo;
        private IVideoSource zrodlo;
        int tryb=0;
        
        public Form1()
        {
            InitializeComponent();
            listaVideo = new FilterInfoCollection(FilterCategory.VideoInputDevice);
            foreach(FilterInfo urzadzenie in listaVideo)
            {
                cmbZrodla.Items.Add(urzadzenie.Name);
            }
            if (cmbZrodla.Items.Count > 0)
            {
                cmbZrodla.SelectedIndex = 0;
            }
            else
            {
                MessageBox.Show("Brak zrodel wideo", "Blad!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            this.Closing += Form1_Closing;
        }

        private void Form1_Closing(object sender, CancelEventArgs e)
        {
            if (zrodlo != null && zrodlo.IsRunning)
            {
                zrodlo.SignalToStop();
            }
        }

        private void nowyObraz(object sender, NewFrameEventArgs eventArgs)
        {
            Bitmap bitmap = new Bitmap((Bitmap)eventArgs.Frame.Clone());
            AForge.Imaging.Filters.ExtractChannel filterC;
            int x, y;
            Color c;
            //pictureBox1.Image = bitmap;
            switch (this.tryb)
            {
                case 0: 
                    pictureBox1.Image = bitmap;
                    break;
                case 1:
                    var filter = AForge.Imaging.Filters.Grayscale.CommonAlgorithms.RMY;
                    bitmap = filter.Apply(bitmap);
                    pictureBox1.Image = bitmap;
                    break;
                case 2:
                    filterC = new AForge.Imaging.Filters.ExtractChannel(RGB.R);
                    bitmap = filterC.Apply(bitmap);
                    pictureBox1.Image = bitmap;
                    break;
                case 3:
                    filterC = new AForge.Imaging.Filters.ExtractChannel(RGB.G);
                    bitmap = filterC.Apply(bitmap);
                    pictureBox1.Image = bitmap;
                    break;
                case 4:
                    filterC = new AForge.Imaging.Filters.ExtractChannel(RGB.B);
                    bitmap = filterC.Apply(bitmap);
                    pictureBox1.Image = bitmap;
                    break;
                case 5:
                    Bitmap temp = new Bitmap(bitmap);
                    int j, k, l;
                    for(x=1;x<temp.Width-1;x++) //nie operujemy na krawedziach
                    {
                        for(y=1;y<temp.Height-1;y++)
                        {
                            j = k = l = 1020;
                            c = bitmap.GetPixel(x - 1, y);
                            j -= c.R;
                            k -= c.G;
                            l -= c.B;
                            c = bitmap.GetPixel(x, y - 1);
                            j -= c.R;
                            k -= c.G;
                            l -= c.B; 
                            c = bitmap.GetPixel(x, y + 1);
                            j -= c.R;
                            k -= c.G;
                            l -= c.B; 
                            c = bitmap.GetPixel(x + 1, y);
                            j -= c.R;
                            k -= c.G;
                            l -= c.B;
                            c = bitmap.GetPixel(x, y);
                            j += 4 * c.R;
                            k += 4 * c.G;
                            l += 4 * c.B;
                            j /= 8;
                            k /= 8;
                            l /= 8;
                            c = Color.FromArgb(j, k, l);
                            temp.SetPixel(x, y, c);
                        }
                    }
                    pictureBox1.Image = temp;
                    break;
                case 6:
                    for (x = 0; x < bitmap.Width; x++)
                    {
                        for (y = 0; y < bitmap.Height; y++)
                        {
                            c = bitmap.GetPixel(x, y);
                            if (c.R + c.G + c.B > 254) bitmap.SetPixel(x, y, Color.FromArgb(255, 255, 255)); //wspolczynniki wybrane ze wzgledu na ciemnosc pokoju
                            else bitmap.SetPixel(x, y, Color.FromArgb(0, 0, 0));
                        }
                    }
                    pictureBox1.Image = bitmap;
                    break;
            }
        }
        private void start(object sender, EventArgs e)
        {
            zrodlo = new VideoCaptureDevice(listaVideo[cmbZrodla.SelectedIndex].MonikerString);
            zrodlo.NewFrame += new NewFrameEventHandler(nowyObraz);
            zrodlo.Start();
        }

        private void zapisz(object sender, EventArgs e)
        {
            if (zrodlo.IsRunning)
            {
                Bitmap snapshot = new Bitmap(pictureBox1.Image);

                snapshot.Save(string.Format(@"C:\Users\Slize\Desktop\snap.png"));
            }
            else
            {
                Console.WriteLine("Nie masz obrazu do zapisania!");
            }
        }

        private void domyslne(object sender, EventArgs e)
        {
            this.tryb = 0;
        }

        private void grey(object sender, EventArgs e)
        {
            this.tryb = 1;
        }

        private void R(object sender, EventArgs e)
        {
            this.tryb = 2;
        }


        private void G(object sender, EventArgs e)
        {
            this.tryb = 4;
        }

        private void B(object sender, EventArgs e)
        {
            this.tryb = 3;
        }


        private void edgy(object sender, EventArgs e)
        {
            this.tryb = 5;
        }

        private void button9_Click(object sender, EventArgs e)
        {
            this.tryb = 6;
        }
    }
}
