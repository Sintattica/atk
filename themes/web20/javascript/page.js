function pageonload()
{
  function roundCorners(arg1,arg2)
  {
    if ($(arg1)) Rico.Corner.round(arg1,arg2);
  }
  roundCorners('box-menu');
  roundCorners('box-menu-title',{compact:true});
  roundCorners('box-menu-content');
  
  roundCorners('box-top');
  roundCorners('box-top-content');
  
  boxes = document.getElementsByClassName('box');
  for (i=0;i<boxes.length;i++)
  {
    roundCorners(boxes[i]);
  }
  
  boxes = document.getElementsByClassName('box-title');
  for (i=0;i<boxes.length;i++)
  {
    roundCorners(boxes[i], {compact:true});
  }
  
  boxes = document.getElementsByClassName('box-content');
  for (i=0;i<boxes.length;i++)
  {
    roundCorners(boxes[i]);
  }
}