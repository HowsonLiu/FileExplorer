# FileExplorer

This is simple file explorer based on Qt.

## What is it like

![UI](ReadMeImage/1.png)    

## Right Click
![Item](ReadMeImage/2.png)    
![blank](ReadMeImage/3.png)                            

## Change View
Icon View:
![Icon View](ReadMeImage/4.png)    
Detail View(sortable):
![Detail View](ReadMeImage/5.png)    
## Copy/Cut/Delete/Paste Action

All Action supports multi-item. Beside, Cut and Copy and Paste are compatible with origin windows explorer.

## Drag and Drop
Only drag and drop is supported if the target is a folder or if the target is blank. The following way is supported: Drag and drop are both from the same FileExplorer's window; Drag from FileExplorer and Drop to others; Drag from FileExplorer and drop to origin windows explorer; Drag from origin windows explorer and drop to FileExplorer. The supported operations are: move operation and copy operation (Link operation is not supported at present). The default operation is the copy operation; In the Icon view, Shift+ drag can be used to move, and in the Detail view, Shift can be pressed while dragging to move.

## More Action
More Action can pop up windows shell context menu, and of cause it supports multi-item.
![More](ReadMeImage/7.png)    

## LAN

File Explorer supports windows shared file(LAN)
![shared file](ReadMeImage/8.png)     

## Rename
You can rename file if you click the selected file
![icon](ReadMeImage/13.png)    
![detail](ReadMeImage/14.png)   


## Error
Wrong url   
![wrong url](ReadMeImage/6.png)   
The file name is same with a dir name   
![same name](ReadMeImage/10.png)   
The target file is exist   
![target existed](ReadMeImage/11.png)   
Target folder is a sub folder of the source folder   
![sub folder](ReadMeImage/12.png)   


## Some bugs
1. Disks with windows system compressed folders are slow to access for the first time
2. Access to nonexistent LAN paths will cause main thread blocked
3. You can edit file's Date Modified or something else in detail view(but it will be never worked :) )
![edit bug](ReadMeImage/16.png)   
4. You can't access the shared folder using the host name(Windows can). It seems to a Qt bug because the QDir is existed but cannot accessed
![sub folder](ReadMeImage/17.png)   
