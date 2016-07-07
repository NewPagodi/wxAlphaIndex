## wxAlphaIndex

This is a small generic wxWidgets generic control that shows a vertical column of letters.  By itself it doesn't do much but it would ordinarily be paired with another control such as a listbox.  The idea is that clicking a letter should result in some action on the list control.  You might have seem a similar control in android apps:

![Android example](https://github.com/NewPagodi/wxAlphaIndex/blob/master/docs/android.jpg "Android example") 

### Screenshots
![Listbox example](https://github.com/NewPagodi/wxAlphaIndex/blob/master/docs/list.png "Listbox example") 
![panel example](https://github.com/NewPagodi/wxAlphaIndex/blob/master/docs/panel.png "panel example")
<p>![contacts example](https://github.com/NewPagodi/wxAlphaIndex/blob/master/docs/contacts.png "contacts example")</p>

### Features
* can be drawn as though it should attach on the left
* or the right
* can use an arbitrary alphabet
* can use an arbitrary ordering of its alphabet
* will show as many letters as possible in the space available
* the letters will be highlighted as the mouse cursor moves over them
* the appearance can be customized to help it blend in with the control it's paired with
 + background
 + font
 + border color
 + separator color
 + text color

### Requirements
* wxWidgets (3.0 or later)

### License
wxWindows Library Licence, Version 3.1