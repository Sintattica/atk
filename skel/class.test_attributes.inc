<?php

  class test_attributes extends atkNode
  {
    function test_attributes()
    {
      $this->atkNode("test_attributes"); // node() constructor is *not* called automatically!
     
      $this->add(new atkAttribute("Attribute",AF_HIDE|AF_PRIMARY|AF_AUTO_INCREMENT));
      $this->add(new atkBoolAttribute("Boolean",AF_SEARCHABLE));
      $this->add(new atkDateAttribute("Date"));
      $this->add(new atkDummyAttribute("This is a dummy attribute"));
      $this->add(new atkFileAttribute("File",array("themes","http://localhost/themes")));
      $this->add(new atkHtmlAttribute("HTML"));
      $this->add(new atkLimitedTextAttribute("LimitedText"));
      $this->add(new atkListAttribute("List",array("Option 1","Option 2","Option 3"),array("option1","option2","option3")));
      $this->add(new atkMultipleFileAttribute("MultipleFile",array("themes","http://locahost/themes")));
      $this->add(new atkNumberAttribute("Number"));
      $this->add(new atkParserAttribute("Parser","Test"));
      $this->add(new atkPasswordAttribute("Password"));
      $this->add(new atkTextAttribute("Text",AF_LIST_HIDE));
      $this->add(new atkTimeAttribute("Time"));
      
      
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