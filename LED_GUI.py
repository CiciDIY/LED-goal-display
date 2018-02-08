import serial
import wx
import numpy

class Example(wx.Frame):
      
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent=parent)    
        self.Show(True)
    
        #self = wx.Panel(self)

        mon_goal = wx.StaticText(self, label="Monthly goal")
        mon_goal_visual = wx.StaticText(self, label="R0")
        cur_total = wx.StaticText(self, label="Current total")
        cur_total_visual = wx.StaticText(self, label="R0")

        self.tc1 = wx.TextCtrl(self)
        self.tc2 = wx.TextCtrl(self)

        submit_button = wx.Button(self, wx.ID_ANY, label = "Submit values")
        submit_button.Bind(wx.EVT_BUTTON, self.submit)

        main_sizer = wx.BoxSizer(wx.VERTICAL)
        goal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        total_sizer = wx.BoxSizer(wx.HORIZONTAL)
        button_sizer = wx.BoxSizer(wx.HORIZONTAL)

        goal_sizer.Add(mon_goal, 0, wx.ALL, 5)
        goal_sizer.Add(self.tc1, 0, wx.ALL, 5)
        goal_sizer.Add(mon_goal_visual, 0, wx.ALL, 5)

        total_sizer.Add(cur_total, 0, wx.ALL,5)
        total_sizer.Add(self.tc2, 0, wx.ALL,5)
        total_sizer.Add(cur_total_visual, 0, wx.ALL,5)

        button_sizer.Add(submit_button, 0, wx.ALL|wx.EXPAND,5)

        main_sizer.Add(goal_sizer,0, wx.ALL|wx.EXPAND, 5)
        main_sizer.Add(total_sizer,0, wx.ALL|wx.EXPAND, 5)
        main_sizer.Add(button_sizer,0, wx.ALL|wx.EXPAND, 5)
        self.SetSizer(main_sizer)
        main_sizer.Fit(self)
        self.Centre()

    def submit(self,event):
        goals = self.tc1.GetValue()
        current_value = self.tc2.GetValue()
        ser = serial.Serial('Com10', 115200)
        ser.open()
        ser.write(chr(goals))
if __name__ == '__main__':
  
    app = wx.App()
    Example(None, title='Monthly goal Display GUI')
    app.MainLoop()