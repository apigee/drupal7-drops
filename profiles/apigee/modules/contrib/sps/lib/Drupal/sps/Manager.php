<?php
namespace Drupal\sps;

/**
 * The Manager is the heart of the SPS system, taking inputs from different
 * parts of the system and pushing them to the correct object for processing
 * it can be organized in to a few different sections
 *
 * Controller Access
 * The Manager manages access to drupal systems via different controllers. The
 * SPS system use the manger to access there when they need access to Drupal
 *  .---------------------------------------------------------------.
 *  |                            Systems                            |
 *  '---------------------------------------------------------------'
 *                                  |
 *                                  v
 *                             .---------.
 *                             | Manager |
 *                             |---------|
 *                             '---------'
 * .---------------------------.    |
 * |     State Controller      |    |    .---------------------------.
 * |---------------------------|    |    |     Plugin Controller     |
 * | Controls the interface    |    |    |---------------------------|
 * | to the State cache        |<---|    | Controls the interface    |
 * | used to hold the current  |    '--->| to the plugin system      |
 * | site state                |    |    | holds method for getting  |
 * '---------------------------'    |    | pluign info and objects   |
 * .---------------------------.    |    '---------------------------'
 * |     Config Controller     |    |
 * |---------------------------|    |    .---------------------------.
 * | Controls the interface    |    |    |    Override Controller    |
 * | to the config for sps     |<---|    |---------------------------|
 * | hold the root condition   |    '--->| Controls the interface    |
 * | and infomation of plugins |         | to the store of the       |
 * '---------------------------'         | current overrides         |
 *                                       '---------------------------'
 *
 * Site State
 * THe Manager can create a site state object, and uses the State Controller
 * to keep it around from page load to page load. When creating site state it
 * hand off the Override Controller So that the Site state can Compile the
 * override data and store it in the Override Controller
 *
 *  @TODO this part of the system should be reviews when we start needing
 *  access to the site state
 *
 * Plugins
 * The Manager is a pass-through to the plugin controller
 */
class Manager {
  protected $state_controller_site_state_key = 'sps_site_state_key';
  protected $state_controller;
  protected $config_controller;
  protected $hook_controller;
  protected $root_condition;
  protected $plugin_controller;
  protected $override_controller;

  /**
   * Constructor for \Drupal\sps\Manager
   *
   * Load the Hook, Plugin and State controller form the configuration.
   *
   * @param \Drupal\sps\StorageControllerInterface $config_controller
   *   the control to be used when accessing config
   *
   * @return \Drupal\sps\Manager
   */
  public function __construct(StorageControllerInterface $config_controller) {

    $this->setConfigController($config_controller)
      ->setHookController($this->createControllerFromConfig(SPS_CONFIG_HOOK_CONTROLLER))
      ->setPluginController($this->createControllerFromConfig(SPS_CONFIG_PLUGIN_CONTROLLER))
      ->setStateController($this->createControllerFromConfig(SPS_CONFIG_STATE_CONTROLLER));
  }

  /**
   * Create a Controller Object based upon a configuration key
   *
   * @param $key
   *  The key from the configuration array that contains the controller information.
   *
   * @return StateControllerInterface|PluginControllerInterface|HookControllerInterface
   */
  protected function createControllerFromConfig($key) {
    $controller_info = $this->getConfigController()->get($key);
    $controller_class = $controller_info['class'];
    $controller_settings = $controller_info['instance_settings'];
    return new $controller_class($controller_settings, $this);
  }

  /**
   * store the state controller
   *
   * @param \Drupal\sps\StateControllerInterface $controller
   *   The control to use when accessing State info (like site state)
   *
   * @return \Drupal\sps\Manager
   *   Self
   */
  protected function setStateController(StateControllerInterface $controller) {
    $this->state_controller = $controller;
    return $this;
  }

  /**
   * store the config controller
   *
   * @param \Drupal\sps\StorageControllerInterface $controller
   *  the control to be used when accessing config
   *
   * @return \Drupal\sps\Manager
   *           Self
   */
  protected function setConfigController(StorageControllerInterface $controller) {
    $this->config_controller = $controller;
    return $this;
  }

  /**
   * store the override controller
   *
   * @param \Drupal\sps\StorageControllerInterface $controller
   *   the control to use when accessing overrides
   *
   * @return \Drupal\sps\Manager
   *   Self
   */
  protected function setOverrideController(StorageControllerInterface $controller) {
    $this->override_controller = $controller;
    return $this;
  }

  /**
   * store the override controller
   *
   * @param \Drupal\sps\PluginControllerInterface $controller
   *   The control to use when accessing plugins
   *
   * @return \Drupal\sps\Manager
   *   Self
   */
  protected function setPluginController(PluginControllerInterface $controller) {
    $this->plugin_controller = $controller;
    return $this;
  }

  /**
   * store the hook controller
   *
   * @param \Drupal\sps\HookControllerInterface $controller
   *   The control to use when accessing drupal invoke and alter function
   * @return \Drupal\sps\Manager
   *   Self
   */
  protected function setHookController(HookControllerInterface $controller) {
    $this->hook_controller = $controller;
    return $this;
  }

  /**
   * store the config controller
  /**
   * Pull the site state form site state controller
   *
   * Note the state controller is responsible for reasonable caching of the site state
   *
   * @return \Drupal\sps\SiteState | NULL
   */
  public function getSiteState() {
    if ($this->state_controller->exists()) {
      return $this->state_controller->get();
    }

    return NULL;
  }

  /**
   * Create A SiteState form an override, and store it.
   *
   * This might get made private
   *
   * @param \Drupal\sps\Plugins\ConditionInterface $condition
   *
   * @return \Drupal\sps\Manager
   *           Self
   */
  public function setSiteState(\Drupal\sps\Plugins\ConditionInterface $condition) {
    $controller_map = $this->getOverrideControllerMap();
    $site_state_class = $this->getConfigController()->get(SPS_CONFIG_SITESTATE);
    $site_state = new $site_state_class($condition, $controller_map);
    $this->state_controller->set($site_state);
    return $this;
  }

  public function clearSiteState() {
    $this->getStateController()->clear();
    return $this;
  }

  protected function getActiveReactionInfo() {
    return $this->getPluginInfo('reaction');
  }

  /**
   * product a map from controller api to override controller instances
   *
   * we start with a list of apis need by reations
   * then we add to it the first override controller we come to for apis
   * not in the config. Also if a controller implements 2 apis we do not
   * create two instances but instead point to the same one.
   *
   * @return array
   */
  protected function getOverrideControllerMap() {

    $controllers = array();
    $controllers_instances = array();
    $instances = array();

    //find all need apis
    foreach($this->getActiveReactionInfo() as $info) {
      $controllers[$info['use_controller_api']] = NULL;
    }
    $config = $this->getConfigController()->exists(SPS_CONFIG_OVERRIDE_CONTROLLERS) ?
      $this->getConfigController()->get(SPS_CONFIG_OVERRIDE_CONTROLLERS) : array();
    $infos = $this->getPluginInfo('override_controller');

    //if the config has valid controllers use them
    foreach($config as $api=>$name) {
      if (isset($infos[$name])) {
        $controllers[$api]  = $name;
      }
    }
    foreach($controllers as $api => $name) {
      if(!$name) {
        foreach($infos as $info_name => $info) {
          //if we have not found a controller yet lets
          //see if this one implements the api
          if(!$name) {
            $imp = is_array($info['implements_controller_api'])
              ? $info['implements_controller_api']
              : array($info['implements_controller_api']);
            foreach ($imp as $imp_api) {
              if($imp_api == $api) {
                $name = $info_name;
              }
            }
          }
        }
      }
      if($name) {
        if (!isset($instances[$name])) {
          $instances[$name] = $this->getPlugin('override_controller', $name);
        }
        $controllers_instances[$api] = $instances[$name];
      }
    }
    return $controllers_instances;
  }


  /**
   * Get what should be a relatively static variable used for storing the site state
   *
   * This is mostly used for tests
   *
   * @return String
   *   the controller key, a string
   */
  public function getStateControllerSiteStateKey() {
    return $this->state_controller_site_state_key;
  }


  /**
   * Passthrough from Drupal form to the correct condition for building the preview form
   *
   * @return array|mixed
   *  A drupal form array created by the root condition
   */
  public function getPreviewForm() {
    $root_condition = $this->getRootCondition();

    return $this->getHookController()->drupalGetForm('sps_condition_preview_form', $root_condition);
  }

  /**
   * Notify the manager that the preview form submission is complete.
   *
   * @param Plugins\ConditionInterface $root_condition
   *
   * @return \Drupal\sps\Manager
   *  Self
   */
  public function previewFormSubmitted(\Drupal\sps\Plugins\ConditionInterface $root_condition) {
    $this->setSiteState($root_condition);
    return $this;
  }

  /**
  * Helper method for getting and causing the root Condition
  *
  * The Root condition is the use as the basis for the constructing the preview form
  * It can be expect that it will be much more complicated then the other conditions
  *
  * This method select the condition and its config using the config controller.
  *
  * @return \Drupal\sps\Plugins\ConditionInterface
  *   the current root condition object
  */
  protected function getRootCondition() {
    if ($site_state = $this->getSiteState()) {
      return $site_state->getCondition();
    }
    $settings = $this->config_controller->get(SPS_CONFIG_ROOT_CONDITION);
    $root_condition_plugin = $settings['name'];
    return  $this->getPlugin('condition', $root_condition_plugin);
  }

  /**
   * call a reaction rect method
   *
   * The manager is use as an interface for Drupal hooks that need to have a
   * reaction react
   *
   * ___________________
   * \                  \
   *  \  Manager::react  \
   *   ) called           )----Reaction Name and Data
   *  /                  /     |
   * /__________________/      |
   *            ^              v
   *            |      ______________
   *            |      \             \
   *            |       \  Check if   \
   *       On Admin Page-) on admin    )
   *            |       /             /
   *            |      /_____________/
   *            |              |
   *            |              v
   *            |      ______________
   *            |      \             \                        .------------------.
   *            |       \  Get Site   \                       | State Controller |
   *       No SiteState--) State       )--------------------->|------------------|
   *            |       /             / ^                     | get()            |
   *            |      /_____________/  |                     '------------------'
   *            |              |        |                               |
   *            |              v        '------SiteState object---------'
   *            |    _________________
   *            |    \                \
   *            |     \  Get Reaction  \
   *      No Reaction  ) ORC api key    )---------reaction name---------.
   *            |     /  and object    / ^                              v
   *            |    /________________/  |                    .-------------------.
   *            |              |         |                    | Plugin Controller |
   *            |              |         |                    |-------------------|
   *            |              |         |                    | getPluginInfo()   |
   *            |              |         |                    | getPlugin()       |
   *            |              |         |                    '-------------------'
   *            |              v         |                              |
   *            |    _________________   '--Override Controller API Key-'
   *            |    \                \     Reaction Object
   *            |     \  Get Override  \
   *          No ORC---) Controller     )-Override Controller API Key-.
   *            |     /                / ^                            v
   *            |    /________________/  |               .-------------------------.
   *            |              |         |               |       Site State        |
   *            |              |         |               |-------------------------|
   *            |              |         |               | getOverrideController() |
   *            |              |         |               '-------------------------'
   *            |              |         |                            |
   *            |              v         '-OverrideController object -'
   *            |        _________
   *            |        \        \
   *            |         \  Call  \
   *     retrun data-------) React  )---data and OverrideController obj-.
   *                      /        /                                    v
   *                     /________/                           .------------------.
   *                           ^                              |     Reaction     |
   *                           |                              |------------------|
   *                           |                              | react()          |
   *                           |                              '------------------'
   *                           |                                        |
   *                           '--------retrun data---------------------'
   *
   *
   * @param String $reaction
   *   the name of a reaction plugin;
   * @param mixed $data
   *   data to be passed to the react method
   *
   * @return mixed
   *   Data used by the item calling reaction
   */
  public function react($reaction, $data) {
    $infos = $this->getActiveReactionInfo();
    if(isset($infos[$reaction]) &&
       !sps_drupal()->path_is_admin(sps_drupal()->current_path()) &&
       ($site_state = $this->getSiteState()) &&
       ($controller = $site_state->getOverrideController($infos[$reaction]['use_controller_api']))
      ) {
      return $this->getPlugin("reaction", $reaction)->react($data, $controller);
    }
  }

  /**
   * factory for building a plugin object
   *
   * @param String $type
   *   the type of plugin as defined in hook_sps_plugin_types_info
   * @param String $name
   *   the name of the plugin as defined in hook_sps_PLUGIN_TYPE_plugin_info;
   *
   * @param Array $settings
   *   an array that should be used for instance settings (instance settings from the plugin info are
   *   add to this array
   *
   * @return \Drupal\sps\Plugins\PluginInterface
   *   An instance of the requested Plugin
   */
  public function getPlugin($type, $name, $settings = NULL) {
    return $this->plugin_controller->getPlugin($type, $name, $this, $settings);
  }

  /**
   * get meta info on a plugin
   * @param String $type
   *   the type of plugin as defined in hook_sps_plugin_types_info
   * @param String | Null $name
   *   the name of the plugin as defined in hook_sps_PLUGIN_TYPE_plugin_info;
   *
   * @return Array
   *   an array of meta data for the plugin
   */
  public function getPluginInfo($type, $name=NULL) {
    return $this->plugin_controller->getPluginInfo($type, $name);
  }

  /**
   * get meta info on a plugin
   *
   * @param String $type
   *   the type of plugin as defined in hook_sps_plugin_types_info
   * @param String $property
   *   the meta property to compare to the value
   * @param mixed $value
   *   the value to compare to the meta property
   *
   * @return Array
   *   an array of meta data for the plugins
   */
  public function getPluginByMeta($type, $property, $value) {
    return $this->plugin_controller->getPluginByMeta($type, $property, $value);
  }

  /**
   * Get the hook controller
   *
   * @return HookControllerInterface
   */
  public function getHookController() {
    return $this->hook_controller;
  }

  /**
   * Get the Plugin Controller
   *
   * @return PluginControllerInterface
   */
  public function getPluginController() {
    return $this->plugin_controller;
  }

  /**
   * Get the State Controller
   *
   * @return StateControllerInterface
   */
  public function getStateController() {
    return $this->state_controller;
  }

  /**
   * Get the config Controller
   *
   * @return StorageControllerInterface
   */
  public function getConfigController() {
    return $this->config_controller;
  }

}
