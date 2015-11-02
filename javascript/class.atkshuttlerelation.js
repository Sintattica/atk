function shuttle_selectAll(id)
{
    var el = document.getElementById(id);
    var options = el.options;

    for (var i = 0; i < options.length; i++)
    {
        options[i].selected = true;
    }
    return true;
}

function shuttle_change(name)
{
    changefunction = name + '_onChange';
    eval('if (window.' + changefunction + ') window.' + changefunction + '();');
}

function shuttle_move(id1, id2, name)
{
    var el1 = document.getElementById(id1);
    var el2 = document.getElementById(id2);

    var newel = el1.cloneNode(false);
    newel.options.length = 0;

    for (var i = 0; i < el1.options.length; i++)
    {
        if (el1.options[i].selected &&
            (window.getComputedStyle(el1.options[i])).display !== 'none') // move options only if not hidden because of filters
        {
            el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
        }
        else
        {
            newel.options[newel.options.length] = new Option(el1.options[i].text, el1.options[i].value);
            // remember if option is hidden or not
            newel.options[newel.options.length - 1].setStyle({
                display : (window.getComputedStyle(el1.options[i])).display !== 'none' ? 'block' : 'none'
            });
        }
    }

    el1.options.length = 0;
    for (i = 0; i < newel.options.length; i++)
    {
        el1.options[el1.options.length] = new Option(newel.options[i].text, newel.options[i].value);
        el1.options[el1.options.length - 1].setStyle({
            display: newel.options[i].getStyle('display')
        });
    }
    shuttle_change(name);
}

function shuttle_moveall(id1, id2, name)
{
    var el1 = document.getElementById(id1);
    var el2 = document.getElementById(id2);

    for (var i = 0; i < el1.options.length; i++)
    {
        if((window.getComputedStyle(el1.options[i])).display !== 'none'){
            // move options only if not hidden because of filters
            el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
            el1.removeChild(el1.options[i]);
            i--;
        }
    }

    shuttle_change(name);
}

