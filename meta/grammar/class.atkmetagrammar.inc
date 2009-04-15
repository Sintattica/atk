<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage meta
 *
 * @copyright (c) 2004-2005 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6323 $
 * $Id$
 */
 
/**
 * The basic (english) "grammar" rules.
 *
 * The singularize and pluralize methods are inspired by the methods with
 * the same name in the Inflector module of the Ruby on Rails framework
 * (Copyright � 2004 David Heinemeier Hansson). The Ruby on Rails framework
 * can be found at http://www.rubyonrails.com
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage meta
 */
class atkMetaGrammar
{
  /**
   * Grammar instances.
   *
   * @var array instances
   */
  static $instances;
  
  /**
   * Returns an instance of the meta grammar with the given class. If no class
   * is specified the default meta grammar is used determined using the 
   * $config_meta_grammar variable.
   *
   * @param string $class full ATK grammar class path
   * 
   * @return atkMetaGrammar meta grammar
   */
  public static function get($class=null)
  {
    if (!is_string($class) || strlen($class) == 0)
    {
      $class = atkconfig("meta_grammar", "atk.meta.grammar.atkmetagrammar");
    }
    
    if (!isset(self::$instances[$class]))
    {
      self::$instances[$class] = atknew($class);
    }
    
    return self::$instances[$class];
  }  
  
  /**
   * Transform the given word using the given rules. As soon as
   * a rule matches the given word, the word, transformed using 
   * the replacement, is returned.
   *
   * @param string $word  word to be transformed
   * @param array $rules list of rules
   *
   * @return transformed word
   */
  public function transform($word, $rules)
  {
    foreach ($rules as $rule => $replacement)
      if (preg_match($rule, $word))
        return preg_replace($rule, $replacement, $word);
    return $word;
  }

  /**
   * Returns the list of singular rules.
   *
   * @return list of singular rules
   */
  public function getSingularRules()
  {
    return array(
      '/(f)ish$/i' => '\1\2ish',
      '/(x|ch|ss|sh)es$/i' => '\1',
      '/(m)ovies$/i' => '\1\2ovie',
      '/(s)eries$/i' => '\1\2eries',
      '/([^aeiouy]|qu)ies$/i' => '\1y',
      '/([lr])ves$/i' => '\1f',
      '/(tive)s$/i' => '\1',
      '/([^f])ves$/i' => '\1fe',
      '/((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)ses$/i' => '\1\2sis',
      '/([ti])a$/i' => '\1um',
      '/(p)eople$/i' => '\1\2erson',
      '/(m)en$/i' => '\1\2an',
      '/(s)tatus$/i' => '\1\2tatus',
      '/(c)hildren$/i' => '\1\2hild',
      '/(n)ews$/i' => '\1\2ews',
      '/s$/i' => ''    
    );
  }

  /**
   * Returns the list of plural rules.
   *
   * @return list of plural rules
   */
  public function getPluralRules()
  {
    return array(
      '/(fish)$/i' => '\1\2',                  // fish
      '/(x|ch|ss|sh)$/i' => '\1es',            // search, switch, fix, box, process, address
      '/(series)$/i' => '\1\2',
      '/([^aeiouy]|qu)ies$/i' => '\1y',
      '/([^aeiouy]|qu)y$/i' => '\1ies',        // query, ability, agency
      '/(?:([^f])fe|([lr])f)$/i' => '\1\2ves', // half, safe, wife
      '/sis$/i' => 'ses',                      // basis, diagnosis
      '/([ti])um$/i' => '\1a',                 // datum, medium
      '/(p)erson$/i' => '\1\2eople',           // person, salesperson
      '/(m)an$/i' => '\1\2en',                 // man, woman, spokesman
      '/(c)hild$/i' => '\1\2hildren',          // child
      '/s$/i' => 's',                          // no change (compatibility)
      '/$/' => 's'
    );
  }

  /**
   * Singularize the given word using the singular rules.
   *
   * @param string $word word to be singularized 
   *
   * @return singularized word
   */
  public function singularize($word)
  { 
    static $rules = NULL;
    if ($rules == NULL)
      $rules = $this->getSingularRules();
    return $this->transform($word, $rules);
  }
  
  /**
   * Pluralize the given word using the plural rules.
   *
   * @param string $word word to be pluralized
   *
   * @return pluralized word
   */
  public function pluralize($word)
  {
    static $rules = NULL;
    if ($rules == NULL)
      $rules = $this->getPluralRules();
    return $this->transform($word, $rules);
  }
}