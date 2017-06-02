<?php
useattrib("atktextattribute");

class test extends atkNode
{

    function test()
    {
        atkNode::__construct("test");

        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("name", AF_OBLIGATORY | AF_UNIQUE));
        $this->add(new atkTextAttribute("description"));

        $this->setOrder("name");
        $this->setTable("test_test");
        $this->setDescriptorTemplate("[name]");
    }

}

?>
