object EditForm: TEditForm
  Left = 192
  Top = 107
  Width = 545
  Height = 391
  Caption = 'Edit script'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnMouseDown = FormMouseDown
  PixelsPerInch = 96
  TextHeight = 13
  object Script: TMemo
    Left = 0
    Top = 26
    Width = 537
    Height = 319
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    HideSelection = False
    Lines.Strings = (
      '# You control Ultima Online client through the global class UO'
      '# It has methods:'
      '#     UO.Print("string")  - print message to the client window.'
      '#     UO.Exec("command")  - execute a command. For the list of'
      '#                           commands consult documentation.'
      
        '#     UO.Count("type",["color"]) - returns count of items of typ' +
        'e'
      '#     UO.Say("something") - make you character say something.'
      '#     UO.Press(KeyCode[,Count[,Delay]]) - Simulate keypress.'
      '#          KeyCode    - Virtual key code.'
      '#          Count      - Optional. Number of keypresses.'
      '#          Delay      - Time in msec to wait between keypresses.'
      '#     UO.LClick(X,Y)      - Simulate left button click.'
      '#     UO.RClick(X,Y)      - Simulate right button click.'
      '#     UO.LDblClick(X,Y)   - Simulate left button double click.'
      '#     UO.RDblClick(X,Y)   - Simulate right button double click.'
      '#     UO.Drag(X1,Y1, X2,Y2 [,amount])   - drag something.'
      '# If you don'#39't specify amount the whole stock is dragged.'
      '# You can use "Insert key code" button to get virtual key code.'
      '# And use "Pick coord" to find mouse coordinates on screen.'
      '# Don'#39't forget to call function Wait(time in msec) after'
      '# using skill/spell to wait until it finishes!'
      '# List of read-only properties:'
      '#     UO.Life, UO.Mana, UO.Stamina, UO.STR, UO.INT, UO.DEX,'
      '#     UO.Weight, UO.Armor, UO.Gold, UO.BM, UO.BP, UO.GA, UO.GS,'
      '#     UO.MR, UO.NS, UO.SA, UO.SS, UO.VA, UO.EN, UO.WH, UO.FD,'
      '#     UO.BR, UO.H, UO.C, UO.M, UO.L, UO.B, UO.AR, UO.BT'
      ''
      'sub main()'
      #9'UO.Print("Hello from function MAIN")'
      'end sub')
    ParentFont = False
    PopupMenu = PopupMenu1
    ScrollBars = ssBoth
    TabOrder = 0
    WantTabs = True
    WordWrap = False
    OnClick = ScriptClick
    OnKeyUp = ScriptKeyUp
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 345
    Width = 537
    Height = 19
    Panels = <
      item
        Text = 'Line: 1 Col: 1'
        Width = 100
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object ControlBar1: TControlBar
    Left = 0
    Top = 0
    Width = 537
    Height = 26
    Align = alTop
    AutoDrag = False
    AutoSize = True
    BevelEdges = []
    BevelInner = bvNone
    BevelOuter = bvNone
    BevelKind = bkNone
    TabOrder = 2
    object Panel1: TPanel
      Left = 95
      Top = 2
      Width = 159
      Height = 22
      Alignment = taLeftJustify
      BevelOuter = bvNone
      Caption = 'Pick coord:'
      TabOrder = 0
      object SpeedButton1: TSpeedButton
        Left = 136
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Insert coordinates into script'
        Caption = 'Ã'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -17
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton1Click
      end
      object SpeedButton2: TSpeedButton
        Left = 112
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Select coordinates on screen'
        Caption = '±'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -17
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton2Click
      end
      object Edit1: TEdit
        Left = 56
        Top = 1
        Width = 56
        Height = 21
        TabOrder = 0
        Text = '0,0'
      end
    end
    object Panel2: TPanel
      Left = 267
      Top = 2
      Width = 121
      Height = 22
      Alignment = taLeftJustify
      BevelOuter = bvNone
      Caption = 'Key code:'
      TabOrder = 1
      object SpeedButton3: TSpeedButton
        Left = 98
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Insert key code into script'
        Caption = 'Ã'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -17
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton3Click
      end
      object Edit2: TEdit
        Left = 88
        Top = 0
        Width = 9
        Height = 21
        TabOrder = 1
        Text = 'Edit2'
        Visible = False
      end
      object HotKey: THotKey
        Left = 50
        Top = 1
        Width = 47
        Height = 20
        HotKey = 9
        InvalidKeys = [hcShift, hcCtrl, hcAlt, hcShiftCtrl, hcShiftAlt, hcCtrlAlt, hcShiftCtrlAlt]
        Modifiers = []
        TabOrder = 0
        TabStop = False
      end
    end
    object Panel3: TPanel
      Left = 11
      Top = 2
      Width = 71
      Height = 22
      BevelOuter = bvNone
      TabOrder = 2
      object SpeedButton5: TSpeedButton
        Left = 24
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Load script'
        Caption = '1'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -15
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton5Click
      end
      object SpeedButton4: TSpeedButton
        Left = 0
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Clear script'
        Caption = 'û'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -27
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton4Click
      end
      object SpeedButton7: TSpeedButton
        Left = 48
        Top = 0
        Width = 23
        Height = 22
        Hint = 'Save script'
        Caption = '<'
        Flat = True
        Font.Charset = SYMBOL_CHARSET
        Font.Color = clNavy
        Font.Height = -19
        Font.Name = 'Wingdings'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = SpeedButton7Click
      end
    end
    object Panel4: TPanel
      Left = 401
      Top = 2
      Width = 133
      Height = 22
      BevelOuter = bvNone
      TabOrder = 3
      object SpeedButton6: TSpeedButton
        Left = 0
        Top = 0
        Width = 71
        Height = 22
        Caption = 'Run function'
        Flat = True
        OnClick = SpeedButton6Click
      end
      object Edit3: TEdit
        Left = 72
        Top = 0
        Width = 60
        Height = 21
        TabOrder = 0
        Text = 'main'
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 504
    Top = 336
    object Cut1: TMenuItem
      Caption = 'Cu&t'
      OnClick = Cut1Click
    end
    object Copy1: TMenuItem
      Caption = '&Copy'
      OnClick = Copy1Click
    end
    object Paste1: TMenuItem
      Caption = '&Paste'
      OnClick = Paste1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object UOPrint1: TMenuItem
      AutoHotkeys = maManual
      Caption = '&Server'
      object UOPrint2: TMenuItem
        Caption = 'UO.Print'
        OnClick = UOInsertText
      end
      object UOExec2: TMenuItem
        Caption = 'UO.Exec'
        OnClick = UOInsertText
      end
      object UOSay2: TMenuItem
        Caption = 'UO.Say'
        OnClick = UOInsertText
      end
      object UOCount1: TMenuItem
        Caption = 'UO.Count'
        OnClick = UOInsertText
      end
    end
    object UOExec1: TMenuItem
      AutoHotkeys = maManual
      Caption = 'C&lient'
      object UOPress1: TMenuItem
        Caption = 'UO.Press'
        OnClick = UOInsertText
      end
      object UOLClick1: TMenuItem
        Caption = 'UO.LClick'
        OnClick = UOInsertText
      end
      object UORClick1: TMenuItem
        Caption = 'UO.RClick'
        OnClick = UOInsertText
      end
      object UOLDblClick1: TMenuItem
        Caption = 'UO.LDblClick'
        OnClick = UOInsertText
      end
      object UORDblClick1: TMenuItem
        Caption = 'UO.RDblClick'
        OnClick = UOInsertText
      end
      object UODrag1: TMenuItem
        Caption = 'UO.Drag'
        OnClick = UOInsertText
      end
    end
    object Other1: TMenuItem
      AutoHotkeys = maManual
      Caption = '&Other'
      object Wait1: TMenuItem
        Caption = 'Wait'
        OnClick = UOInsertText
      end
      object MessageBox1: TMenuItem
        Caption = 'MessageBox'
        OnClick = UOInsertText
      end
      object Str1: TMenuItem
        Caption = 'Str'
        OnClick = UOInsertText
      end
      object Val1: TMenuItem
        Caption = 'Val'
        OnClick = UOInsertText
      end
      object Len1: TMenuItem
        Caption = 'Len'
        OnClick = UOInsertText
      end
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Undo1: TMenuItem
      Caption = '&Undo'
      OnClick = Undo1Click
    end
  end
end
