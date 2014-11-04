﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Drawing;
using System.Collections;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace serial
{

    public partial class conDialog : Form
    {
        public conDialog()
        {
            InitializeComponent();
            this.StartPosition = FormStartPosition.CenterParent;
        }

        private void btn_open_Click(object sender, EventArgs e)
        {
            if (txt_Port.Text != "")
            {
                SerialPort sp1 = new SerialPort(txt_Port.Text, Convert.ToInt32(txt_Speed.Text), Parity.None, 8, StopBits.One);

                // Try to open Port 
                try
                {
                    sp1.Open();
                }
                catch
                {
                    MessageBox.Show(sp1.PortName + "Open Port ERROR", "Warning");
                    Application.Exit();
                }

                if (sp1.IsOpen)
                {
                    CHConn.isConnected = true;
                    CHConn.ConnObject = sp1;
                    CHConn.linkInfoString = sp1.PortName.ToString() + " " + sp1.BaudRate.ToString();
                    this.DialogResult = System.Windows.Forms.DialogResult.OK;
                }
            }
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Application.Exit();
        }

        private void conDialog_Load(object sender, EventArgs e)
        {
            btn_Refresh_Click(sender, e);
        }

        private void btn_Refresh_Click(object sender, EventArgs e)
        {
            // get serial
            ArrayList PortNames = new ArrayList();
            lbl_AvailPorts.Text = "Available Ports:";
            foreach (string Port in SerialPort.GetPortNames())
            {
                PortNames.Add(Port);
                lbl_AvailPorts.Text += "  " + Port;
            }
            
            if (PortNames.Count != 0) txt_Port.Text = (string)PortNames[0];
        }

        //  window cannot move
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == 0x00A1 && m.WParam.ToInt32() == 2)
            {
                m.Msg = 0x0201;
                m.LParam = IntPtr.Zero;
            }
            base.WndProc(ref m);
        }

    }

    static public class CHConn
    {
        static public bool isConnected;
        static public object ConnObject;
        static public string linkInfoString;
    }

}
