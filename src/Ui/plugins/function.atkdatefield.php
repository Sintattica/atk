<?php

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\DateAttribute;

/**
 * Function for embedding a date control in html.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_function_atkdatefield($params, &$smarty)
{
    $name = isset($params['name']) ? $params['name'] : 'date';
    $format = isset($params['format']) ? $params['format'] : Tools::atktext('date_format_edit', 'atk', '', '', '', true);
    $mandatory = isset($params['mandatory']) && $params['mandatory'] || isset($params['obligatory']) && $params['obligatory'];
    $noweekday = isset($params['noweekday']) && $params['noweekday'];
    $calendar = isset($params['calendar']) && $params['calendar'];
    $time = isset($params['time']) ? $params['time'] : ($mandatory ? mktime() : null);
    $min = isset($params['min']) ? $params['min'] : 0;
    $max = isset($params['max']) ? $params['max'] : 0;

    if (is_array($time)) {
        $date = $time;
    } else {
        if ($time == null) {
            $date = null;
        } else {
            if (is_numeric($time)) {
                $date = getdate($time);
                $date = array('day' => $date['mday'], 'month' => $date['mon'], 'year' => $date['year']);
            } else {
                if (preg_match('/([0-9]{1,2})-([0-9]{1,2})-([0-9]{4})/', $time, $matches)) {
                    $date = array('day' => (int)$matches[1], 'month' => (int)$matches[2], 'year' => $matches[3]);
                } else {
                    $date = getdate(strtotime($time));
                    $date = array('day' => $date['mday'], 'month' => $date['mon'], 'year' => $date['year']);
                }
            }
        }
    }

    $attr = new DateAttribute($name, $format, '', $min, $max,
        ($noweekday ? DateAttribute::AF_DATE_EDIT_NO_DAY : 0) | ($mandatory ? Attribute::AF_OBLIGATORY : 0) | ($calendar ? 0 : DateAttribute::AF_DATE_NO_CALENDAR));
    $html = $attr->edit(array($name => $date));

    return $html;
}
