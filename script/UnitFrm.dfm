object MainForm: TMainForm
  Left = 192
  Top = 107
  ActiveControl = Button1
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 289
  ClientWidth = 270
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 120
    Top = 78
    Width = 44
    Height = 13
    Caption = 'Function:'
  end
  object Label1: TLabel
    Left = 48
    Top = 104
    Width = 115
    Height = 13
    Caption = 'List of running functions:'
  end
  object Label3: TLabel
    Left = 120
    Top = 48
    Width = 117
    Height = 13
    Caption = 'Script parser version: 0.3'
  end
  object Button1: TButton
    Left = 40
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Load'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 160
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 40
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Edit'
    TabOrder = 2
    OnClick = Button3Click
  end
  object Button5: TButton
    Left = 96
    Top = 240
    Width = 75
    Height = 25
    Caption = 'Terminate'
    TabOrder = 3
    OnClick = Button5Click
  end
  object Edit1: TEdit
    Left = 170
    Top = 74
    Width = 65
    Height = 21
    TabOrder = 5
    Text = 'main'
  end
  object Button4: TButton
    Left = 40
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Run'
    TabOrder = 4
    OnClick = Button4Click
  end
  object RunList: TListBox
    Left = 40
    Top = 120
    Width = 193
    Height = 113
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 6
  end
  object OD: TOpenDialog
    DefaultExt = 'sc'
    Filter = 'Scripts (*.sc)|*.sc|All files (*.*)|*.*'
    InitialDir = '.'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Load script'
    Left = 8
    Top = 8
  end
  object SD: TSaveDialog
    DefaultExt = 'sc'
    Filter = 'Scripts (*.sc)|*.sc|All files (*.*)|*.*'
    InitialDir = '.'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Title = 'Save script'
    Left = 128
    Top = 8
  end
end
