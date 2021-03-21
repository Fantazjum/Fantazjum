using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using iText.IO.Image;
using iText.Kernel.Pdf;
using iText.Layout;
using iText.Layout.Element;
using QRCoder;
using static QRCoder.PayloadGenerator;

namespace CzarnoNaBialym
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            pictureBox1.BackColor = Color.White;
            pictureBox2.BackColor = Color.White;
            comboBox1.Items.Insert(0, "Polska");
            comboBox1.Items.Insert(1, "Uzytek wlasny 1");
            comboBox1.Items.Insert(2, "Uzytek wlasny 2");
            comboBox1.Items.Insert(3, "Uzytek wlasny 3");
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBox1.SelectedIndex = 0;
            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Url generated = new Url(textBox1.Text);
            string payload = generated.ToString();

            QRCodeGenerator qrGenerator = new QRCodeGenerator();
            QRCodeData qrCodeData = qrGenerator.CreateQrCode(payload, QRCodeGenerator.ECCLevel.Q);
            QRCode qrCode = new QRCode(qrCodeData);
            Bitmap qrBitmap = qrCode.GetGraphic(20);
            pictureBox1.Image = qrBitmap;
        }

        private String encode(String data)
        {
            String[] L = { "0001101", "0011001", "0010011", "0111101",
                "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
            String[] G = { "0100111", "0110011", "0011011", "0100001",
                "0011101", "0111001", "0000101", "0010001", "0001001", "0010111"};
            String[] R = { "1110010", "1100110", "1101100", "1000010",
                "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};
            char zero = '0';

            int mode = data[0] - zero;
            String transformed = "101";
            for(int i = 1; i < data.Length; i++)
            {
                String temp = "";
                if (i < 7) switch (mode)
                    {
                        case 0:
                            temp = L[data[i] - zero];
                            break;
                        case 1:
                            if (i == 1 || i == 2 || i == 4) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 2:
                            if (i < 3 || i == 5) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 3:
                            if (i == 6 || i < 3) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 4:
                            if (i == 2 || i > 4) temp = G[data[i] - zero];
                            else temp = L[data[i] - zero];
                            break;
                        case 5:
                            if (i == 1 || i == 4 || i == 5) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 6:
                            if (i == 1 || i > 4) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 7:
                            if (i % 2 == 1) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 8:
                            if (i == 1 || i % 3 == 3) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                        case 9:
                            if (i == 1 || i == 4 || i == 6) temp = L[data[i] - zero];
                            else temp = G[data[i] - zero];
                            break;
                    }
                else temp = R[data[i] - zero];
                transformed += temp;
                if (i == 6) transformed += "01010"; //add separator
            }
            transformed += "101";
            return transformed;
        }

        private void draw(String code, String num)
        {
            System.Drawing.Image result = new Bitmap(220, 120);
            Graphics g = Graphics.FromImage(result);
            g.PageUnit = GraphicsUnit.Pixel;
            Pen bar = new Pen(Color.Black, 2);
            for (int i = 0; i < 95; i++)
            {
                if (i < 3 || (i > 44 && i < 50) || i > 91)
                    if (code[i] == '1') g.DrawLine(bar, 19.0F + 2 * i, 3.0F, 19.0F + 2 * i, 95.0F);
                    else continue;
                else
                    if (code[i] == '1') g.DrawLine(bar, 19.0F + 2 * i, 3.0F, 19.0F + 2 * i, 90.0F);
            }
            String first = num[0].ToString();
            String left = num.Substring(1, 6);
            String right = num.Substring(7);
            for(int i = left.Length - 1; i > 0; i--)
            {
                left = left.Insert(i, " ");
                right = right.Insert(i, " ");
            }
            Font used = new Font("Space Mono", 12);
            g.DrawString(first, used,Brushes.Black, 5.0F, 90.0F);
            g.DrawString(left, used, Brushes.Black, 30.0F, 90.0F);
            g.DrawString(right, used, Brushes.Black, 120.0F, 90.0F);
            pictureBox2.Image = result;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            String input = textBox2.Text;
            int control = 0;

            if(input.Length!=12)
            {
                MessageBox.Show("Kod musi mieć długość 12 cyfr!");
                return;
            }

            for(int i = 0; i < 12; i++)
            {
                if(!(input[i]>='0' && input[i]<='9'))
                {
                    MessageBox.Show("Kod jest podawany jako cyfry!");
                    return;
                }
                else
                {
                    control += (input[i] - '0') * (1 + 2 * (i % 2));
                }
            }

            control = (10 - control % 10) % 10;
            textBox5.Text = control.ToString();
            char final = (char)control;
            final += '0';
            input += final;

            draw(encode(input), input);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            String manufacturer = textBox3.Text;
            String product = textBox4.Text;
            String data = "";
            int control = 0;
            int i = 3;
            int j = 0;
            
            switch (comboBox1.SelectedIndex)
            {
                case 0:
                    control = 32;
                    data = "590";
                    break;
                case 1:
                    control = 11;
                    data = "025";
                    break;
                case 2:
                    control = 17;
                    data = "045";
                    break;
                case 3:
                    control = 22;
                    data = "255";
                    break;
            }

            if(textBox3.TextLength < 5)
            {
                MessageBox.Show("Długość kodu producenta musi wynosić przynajmniej 5!");
                return;
            }

            if (textBox3.TextLength + textBox4.TextLength != 9)
            {
                MessageBox.Show("Łączna długość kodu producenta i produktu jest różna od 9!");
                return;
            }

            for(j = 0; j < manufacturer.Length; j++)
            {
                if (!(manufacturer[j] >= '0' && manufacturer[j] <= '9'))
                {
                    MessageBox.Show("Kod jest podawany jako cyfry!");
                    return;
                }
                else
                {
                    control += (manufacturer[j] - '0') * (1 + 2 * (i % 2));
                    i++;
                }
            }

            for (j = 0; j < product.Length; j++)
            {
                if (!(product[j] >= '0' && product[j] <= '9'))
                {
                    MessageBox.Show("Kod jest podawany jako cyfry!");
                    return;
                }
                else
                {
                    control += (product[j] - '0') * (1 + 2 * (i % 2));
                    i++;
                }
            }

            data += manufacturer + product;
            control = (10 - control % 10) % 10;
            textBox5.Text = control.ToString();
            char final = (char)control;
            final += '0';
            data += final;

            draw(encode(data), data);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            PdfWriter writer = new PdfWriter("C:\\Users/Slize/Desktop/SemestrV/UP/lab/Siatka.pdf");
            PdfDocument pdf = new PdfDocument(writer);
            Document document = new Document(pdf);
            System.Drawing.Image kod = pictureBox2.Image;
            ImageData data = ImageDataFactory.Create(kod, Color.White);
            iText.Layout.Element.Image komorka = new iText.Layout.Element.Image(data);
            Table table = new Table(2).UseAllAvailableWidth();

            for (int i = 0; i < 12; i++)
            {
                table.AddCell(komorka);
            }
            document.Add(table);
            document.Close();
        }
    }
}
