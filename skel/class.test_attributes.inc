<?php

  class test_attributes extends atkNode
  {
    function test_attributes()
    {
      $this->atkNode("test_attributes"); // node() constructor is *not* called automatically!
     
      $this->addAttribute(new atkAttribute("Attribute",AF_HIDE|AF_PRIMARY|AF_AUTO_INCREMENT));
      $this->addAttribute(new atkBoolAttribute("Boolean",AF_SEARCHABLE));
      $this->addAttribute(new atkDateAttribute("Date"));
      $this->addAttribute(new atkDummyAttribute("This is a dummy attribute"));
      $this->addAttribute(new atkFileAttribute("File",array("themes","http://localhost/themes")));
      $this->addAttribute(new atkHtmlAttribute("HTML"));
      $this->addAttribute(new atkLimitedTextAttribute("LimitedText"));
      $this->addAttribute(new atkListAttribute("List",array("Option 1","Option 2","Option 3"),array("option1","option2","option3")));
      $this->addAttribute(new atkMultipleFileAttribute("MultipleFile",array("themes","http://locahost/themes")));
      $this->addAttribute(new atkNumberAttribute("Number"));
      $this->addAttribute(new atkParserAttribute("Parser","Test"));
      $this->addAttribute(new atkPasswordAttribute("Password"));
      $this->addAttribute(new atkTextAttribute("Text",AF_LIST_HIDE));
      $this->addAttribute(new atkTimeAttribute("Time"));
      
      
      //$this->setOrder("news_id");
      //$this->setFilter("test='test'");
      
      $this->setTable("test_attributes");
    }

    function initial_values()
    {
       return Array("repostamp"=>"now()","timestamp"=>date("YmdGis"),"birthstamp"=>"now()");
    }

    function edit_values($record)
    {
       atkdebug("EDIT VALUES");
       return Array("hits"=>100);
    }


    function descriptor_def()
    {
      return "[heading]";
    }
  }

?>