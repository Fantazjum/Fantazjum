namespace Kamerka
{
    partial class Form1
    {
        /// <summary>
        /// Wymagana zmienna projektanta.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Wyczyść wszystkie używane zasoby.
        /// </summary>
        /// <param name="disposing">prawda, jeżeli zarządzane zasoby powinny zostać zlikwidowane; Fałsz w przeciwnym wypadku.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Kod generowany przez Projektanta formularzy systemu Windows

        /// <summary>
        /// Metoda wymagana do obsługi projektanta — nie należy modyfikować
        /// jej zawartości w edytorze kodu.
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button8 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.cmbZrodla = new System.Windows.Forms.ComboBox();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.button7 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.button9 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(12, 14);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(481, 343);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // button8
            // 
            this.button8.Location = new System.Drawing.Point(39, 387);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(160, 33);
            this.button8.TabIndex = 2;
            this.button8.Text = "Start";
            this.button8.UseVisualStyleBackColor = true;
            this.button8.Click += new System.EventHandler(this.start);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(281, 387);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(160, 33);
            this.button2.TabIndex = 1;
            this.button2.Text = "Zapisz";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.zapisz);
            // 
            // cmbZrodla
            // 
            this.cmbZrodla.FormattingEnabled = true;
            this.cmbZrodla.Location = new System.Drawing.Point(561, 394);
            this.cmbZrodla.Name = "cmbZrodla";
            this.cmbZrodla.Size = new System.Drawing.Size(192, 21);
            this.cmbZrodla.TabIndex = 0;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(593, 116);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(160, 33);
            this.button3.TabIndex = 2;
            this.button3.Text = "Szarość";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.grey);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(593, 175);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(160, 33);
            this.button4.TabIndex = 3;
            this.button4.Text = "Czerwień";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.R);
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(593, 253);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(160, 33);
            this.button5.TabIndex = 4;
            this.button5.Text = "Błękit";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.B);
            // 
            // button6
            // 
            this.button6.Location = new System.Drawing.Point(593, 214);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(160, 33);
            this.button6.TabIndex = 5;
            this.button6.Text = "Zieleń";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.G);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(593, 14);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(160, 33);
            this.button1.TabIndex = 0;
            this.button1.Text = "Domyślne";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.domyslne);
            // 
            // button7
            // 
            this.button7.Location = new System.Drawing.Point(593, 310);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(160, 33);
            this.button7.TabIndex = 6;
            this.button7.Text = "Krawędzie";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new System.EventHandler(this.edgy);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(590, 368);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(128, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Wybierz urządzenie video";
            // 
            // button9
            // 
            this.button9.Location = new System.Drawing.Point(593, 66);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(160, 33);
            this.button9.TabIndex = 8;
            this.button9.Text = "Binarność";
            this.button9.UseVisualStyleBackColor = true;
            this.button9.Click += new System.EventHandler(this.button9_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.button9);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button8);
            this.Controls.Add(this.cmbZrodla);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button7);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Name = "Form1";
            this.Text = "Kamerka";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        
        private System.Windows.Forms.ComboBox cmbZrodla;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.Button button9;
    }
}

