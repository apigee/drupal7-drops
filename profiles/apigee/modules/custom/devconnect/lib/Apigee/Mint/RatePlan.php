<?php

namespace Apigee\Mint;

use Apigee\Util\CacheFactory;
use Apigee\Util\Log as Log;

use Apigee\Exceptions\ParameterException as ParameterException;
{ // class is inclosed in curly bracets due to an overlapping issue in namespaces. Need to figure out how to avoid it

  class RatePlan extends Base\BaseObject {

    /**
     * Advance
     * @var boolean
     */
    private $advance;

    /**
     * com.apigee.mint.model.Organization
     * @var \Apigee\Mint\Organization
     */
    private $organization;

    /**
     * MonetizationPackage
     * @var \Apigee\Mint\MonetizationPackage
     */
    private $monetization_package;

    /**
     * Rate Plan currency
     * @var \Apigee\Mint\DataStructures\SupportedCurrency
     */
    private $currency;

    /**
     * Reference rate plan id RatePlan
     * @var \Apigee\Mint\RatePlan
     */
    private $parent_rate_plan;

    private $payment_due_days;

    /**
     * References a rate plan that this plan belongs to if any. This is for
     * devconnect internal logic.
     * @var \Apigee\Mint\RatePlan
     */
    private $child_rate_plan;

    /**
     * com.apigee.mint.model.Developer
     * @var \Apigee\Mint\Developer
     */
    private $developer;

    /**
     * com.apigee.mint.model.DeveloperCategory
     * @var \Apigee\Mint\DeveloperCategory
     */
    private $developer_category;

    /**
     * Array of DeveloperRatePlan
     * @var \Apigee\Mint\DeveloperRatePlan
     */
    private $developers = array();

    /**
     * com.apigee.mint.model.ApplicationCategory
     * @var \Apigee\Mint\ApplicationCategory
     */
    private $application_category;

    /**
     * Exchange Organization
     * @var \Apigee\Mint\Organization
     */
    private $exchange_organization; //@TODO Verify if required

    /**
     * Rate plan type.
     * @var string
     */
    private $type;

    /**
     * Name
     * @var string
     */
    private $name;

    /**
     * Display Name
     * @var string
     */
    private $display_name;

    /**
     *  Description
     * @var string
     */
    private $description;

    /**
     * One time set up fee
     * @var double
     */
    private $set_up_fee;

    /**
     * Recurring time set up fee
     * @var double
     */
    private $recurring_fee;

    /**
     * Duration
     * @var int
     */
    private $frequency_duration;

    /**
     * Duration Type.
     * @var string
     */
    private $frequency_duration_type;

    /**
     * Recurring Type Used to define if scheduler needs to be run based on Calendar or Custom (plan start date)
     * Possible values:
     * @var string
     */
    private $recurring_type;

    /**
     * Recurring start unit this is only used if the recurringType is CALENDAR
     * @var int
     */
    private $recurring_start_unit;

    /**
     * Should this package be prorated?
     * @var boolean
     */
    private $prorate;

    /**
     * Early termination fee
     * @var double
     */
    private $early_termination_fee;

    /**
     * Effective Start date
     * @var string
     */
    private $start_date;

    /**
     * Effective End date
     * @var string
     */
    private $end_date;

    /**
     * Freemium duration
     * @var int
     */
    private $freemium_duration;

    /**
     * Freemium number of units
     * @var int
     */
    private $freemium_unit;

    /**
     * Freemium Duration Type
     * @var string
     */
    private $freemium_duration_type;

    /**
     * Is published?
     * @var boolean
     */
    private $published;

    /**
     * Contract duration
     * @var int
     */
    private $contract_duration;

    /**
     * Contract Duration Type .
     * @var string
     */
    private $contract_duration_type;

    /**
     * Keep developer original start date (used for rate plan revisions)
     * @var boolean
     */
    private $keep_original_start_date;

    /**
     * Rate plan details
     * @var array Array must elements must be instances of Apigee\Mint\DataStructures\RatePlanRate
     */
    private $rate_plan_details = array();

    /**
     * Monetization Package id
     * @var string
     */
    private $m_package_id;

    /**
     * @var string
     */
    public $id;

    /**
     * Class constructor
     * @param string $m_package_id Monetization Package id
     * @param \Apigee\Util\APIClient $client
     */
    public function __construct($m_package_id, \Apigee\Util\APIClient $client) {
      $this->init($client);
      $this->m_package_id = $m_package_id;

      $this->base_url = '/mint/organizations/' . rawurlencode($client->getOrg()) . '/monetization-packages/' . rawurlencode($m_package_id) . '/rate-plans';
      $this->wrapper_tag = 'ratePlan';
      $this->id_field = 'id';

      $this->initValues();
    }

    // Override of BaseObject methods

    public function getList($page_num = NULL, $page_size = 20, $current = true, $all_available = true) {
      if (!isset($this->developer)) {
        return parent::getList();
      }

      $current = $current ? 'true' : 'false';
      $all_available = $all_available ? 'true' : 'false';

      $url = '/mint/organizations/' . rawurlencode($this->client->getOrg()) . '/monetization-packages/' . rawurlencode($this->m_package_id) . '/developers/' . rawurlencode($this->developer->getEmail()) . '/rate-plans?current=' . $current . '&allAvailable=' . $all_available;
      $this->client->get($url);
      $response = $this->getResponse();

      $return_objects = array();

      foreach ($response[$this->wrapper_tag] as $response_data) {
        $obj = $this->instantiateNew();
        $obj->loadFromRawData($response_data);
        $return_objects[] = $obj;
      }
      return $return_objects;
    }

    // Implementation of BaseObject abstract methods

    public function instantiateNew() {
      return new RatePlan($this->m_package_id, $this->client);
    }

    public function loadFromRawData($data, $reset = FALSE) {

      if ($reset) {
        $this->initValues();
      }

      $excluded_properties = array(
        'org',
        'mPackageId',
        'organization',
        'monetizationPackage',
        'currency',
        'parentRatePlan',
        'developer',
        'developerCategory',
        'developers',
        'applicationCategory',
        'exchangeOrganization',
        'ratePlanDetails'
      );

      foreach (array_keys($data) as $property) {
        if (in_array($property, $excluded_properties)) {
          continue;
        }

        // form the setter method name to invoke setXxxx
        $setter_method = 'set' . ucfirst($property);
        if (method_exists($this, $setter_method)) {
          $this->$setter_method($data[$property]);
        }
        else {
          Log::write(__CLASS__, Log::LOGLEVEL_NOTICE, 'No setter method was found for property "' . $property . '"');
        }
      }

      // Set objects

      if (isset($data['organization'])) {
        $organization = new Organization($this->client);
        $organization->loadFromRawData($data['organization']);
        $this->organization = $organization;
      }

      if (isset($data['monetizationPackage'])) {
        $monetizationPackage = new MonetizationPackage($this->client);
        $monetizationPackage->loadFromRawData($data['monetizationPackage']);
        $this->monetization_package = $monetizationPackage;
      }

      if (isset($data['currency'])) {
        $this->currency = new DataStructures\SupportedCurrency($data['currency']);
      }

      if (isset($data['parentRatePlan'])) {
        $rate_plan = new RatePlan($this->m_package_id, $this->client);
        $rate_plan->loadFromRawData($data['parentRatePlan']);
        $this->setParentRatePlan($rate_plan);
      }

      if (isset($data['developer'])) {
        $dev = new Developer($this->client);
        $dev->loadFromRawData($data['developer']);
        $this->developer = $dev;
      }

      //@TODO Implement load of developerCategory

      //@TODO Implement load of developers

      //@TODO Implement load of applicationCategory

      if (isset($data['exchangeOrganization'])) {
        $organization = new Organization($this->client);
        $organization->loadFromRawData($data['exchangeOrganization']);
        $this->exchange_organization = $organization;
      }

      if (isset($data['ratePlanDetails'])) {
        foreach ($data['ratePlanDetails'] as $ratePlanDetail) {
          $this->rate_plan_details[] = new DataStructures\RatePlanDetail($ratePlanDetail, $this->client);
        }
      }
    }

    protected function initValues() {
      $this->advance = FALSE;
      $this->organization = NULL;
      $this->monetization_package = NULL;
      $this->currency = NULL;
      $this->child_rate_plan = NULL;
      $this->parent_rate_plan = NULL;
      $this->developer = NULL;
      $this->developer_category = NULL;
      $this->developers = array();
      $this->application_category = NULL;
      $this->exchange_organization = NULL;
      $this->type = '';
      $this->name = '';
      $this->display_name = '';
      $this->description = '';
      $this->set_up_fee = 0;
      $this->recurring_fee = 0;
      $this->frequency_duration = 0;
      $this->frequency_duration_type = '';
      $this->recurring_type = '';
      $this->recurring_start_unit = 0;
      $this->prorate = FALSE;
      $this->early_termination_fee = 0;
      $this->start_date = '';
      $this->end_date = '';
      $this->freemium_duration = 0;
      $this->freemium_unit = 0;
      $this->freemium_duration_type = '';
      $this->published = FALSE;
      $this->contract_duration = 0;
      $this->contract_duration_type = '';
      $this->keep_original_start_date = FALSE;
      $this->rate_plan_details = array();
    }

    public function __toString() {
      // @TODO Make right implementation
      return json_encode($this);
    }

    // getters

    /**
     * Is Advance?
     * @return boolean
     */
    public function isAdvance() {
      return $this->advance;
    }

    /**
     * Get com.apigee.mint.model.Organization
     * @return \Apigee\Mint\Organization
     */
    public function getOrganization() {
      return $this->organization;
    }

    /**
     * Get MonetizationPackage
     * @return \Apigee\Mint\MonetizationPackage
     */
    public function getMonetizationPackage() {
      return $this->monetization_package;
    }

    /**
     * Get Rate Plan currency
     * @return \Apigee\Mint\DataStructures\SupportedCurrency
     */
    public function getCurrency() {
      return $this->currency;
    }

    /**
     * Get Parent Rate Plan
     * @return \Apigee\Mint\RatePlan
     */
    public function getParentRatePlan() {
      return $this->parent_rate_plan;
    }

    public function getPaymentDueDays() {
      return $this->payment_due_days;
    }

    public function getChildRatePlan() {
      return $this->child_rate_plan;
    }

    /**
     * Get com.apigee.mint.model.Developer
     * @return \Apigee\Mint\Developer
     */
    public function getDeveloper() {
      return $this->developer;
    }

    /**
     * Get com.apigee.mint.model.DeveloperCategory
     * @return \Apigee\Mint\DeveloperCategory
     */
    public function getDeveloperCategory() {
      return $this->developer_category;
    }

    /**
     * Get an array of DeveloperRatePlan
     * @return \Apigee\Mint\DeveloperRatePlan
     */
    public function getDeveloperRatePlans() {
      return $this->developers;
    }

    /**
     * Get com.apigee.mint.model.ApplicationCategory
     * @return \Apigee\Mint\ApplicationCategory
     */
    public function getApplicationCategory() {
      return $this->application_category;
    }

    /**
     * Get Exchange Organization
     * @return \Apigee\Mint\Organization
     */
    public function getExchangeOrganization() {
      return $this->exchange_organization;
    }

    /**
     * Get Rate plan type.
     * @return string
     */
    public function getType() {
      return $this->type;
    }

    /**
     * Get Name
     * @return string
     */
    public function getName() {
      return $this->name;
    }

    /**
     * Get Display Name
     * @return string
     */
    public function getDisplayName() {
      return $this->display_name;
    }

    /**
     * Get Description
     * @return string
     */
    public function getDescription() {
      return $this->description;
    }

    /**
     * Get One time set up fee
     * @return double
     */
    public function getSetUpFee() {
      return $this->set_up_fee;
    }

    /**
     * Get Recurring time set up fee
     * @return double
     */
    public function getRecurringFee() {
      return $this->recurring_fee;
    }

    /**
     * Get Frecuency Duration
     * @return int
     */
    public function getFrequencyDuration() {
      return $this->frequency_duration;
    }

    /**
     * Get Frecuency Duration Type.
     * @return string
     */
    public function getFrequencyDurationType() {
      return $this->frequency_duration_type;
    }

    /**
     * Get Recurring Type Used to define if scheduler needs to be run based on Calendar or Custom (plan start date)
     * @return string
     */
    public function getRecurringType() {
      return $this->recurring_type;
    }

    /**
     * Get Recurring start unit this is only used if the recurringType is CALENDAR
     * @return int
     */
    public function getRecurringStartUnit() {
      return $this->recurring_start_unit;
    }

    /**
     * Is this package to be prorated
     * @return boolean
     */
    public function isProrate() {
      return $this->prorate;
    }

    /**
     * Get Early termination fee
     * @return double
     */
    public function getEarlyTerminationFee() {
      return $this->early_termination_fee;
    }

    /**
     * Get Effective Start date
     * @return string
     */
    public function getStartDate() {
      return $this->start_date;
    }

    /**
     * Get Effective End date
     * @return string
     */
    public function getEndDate() {
      return $this->end_date;
    }

    /**
     * Get Freemium duration
     * @return int
     */
    public function getFreemiumDuration() {
      return $this->freemium_duration;
    }

    /**
     * Get Freemium number of units
     * @return int
     */
    public function getFreemiumUnit() {
      return $this->freemium_unit;
    }

    /**
     * Get Freemium Duration Type
     * @return string
     */
    public function getFreemiumDurationType() {
      return $this->freemium_duration_type;
    }

    /**
     * Is published?
     * @return boolean
     */
    public function isPublished() {
      return $this->published;
    }

    /**
     * Get Contract duration
     * @return int
     */
    public function getContractDuration() {
      return $this->contract_duration;
    }

    /**
     * Get Contract Duration Type .
     * @return string
     */
    public function getContractDurationType() {
      return $this->contract_duration_type;
    }

    /**
     * Keep developer original start date (used for rate plan revisions)
     * @return boolean
     */
    public function getKeepOriginalStartDate() {
      return $this->keep_original_start_date;
    }

    /**
     * Get Rate plan details
     * @return array Array must elements must be instances of Apigee\Mint\DataStructures\RatePlanRate
     */
    public function getRatePlanDetails() {
      return $this->rate_plan_details;
    }

    public function getRatePlanDetailsByProduct(Product $product = NULL) {
      if ($product == NULL) {
        return $this->rate_plan_details;
      }
      else {
        $rate_plan_details = array();
        foreach ($this->rate_plan_details as &$rate_plan_detail) {
          if (isset($rate_plan_detail->product) && $rate_plan_detail->product->getId() == $product->getId()) {
            $rate_plan_details[] = $rate_plan_detail;
          }
        }
        return $rate_plan_details;
      }
    }
    //setters

    /**
    * Set Advance
    * @param boolean $advance
    */
    public function  setAdvance($advance) {
      $this->advance = $advance;
    }

    /**
     * Set com.apigee.mint.model.Organization
     * @param \Apigee\Mint\Organization $organization
     */
    public function setOrganization(Organization $organization) {
      $this->organization = $organization;
    }

    /**
     * Set MonetizationPackage
     * @param \Apigee\Mint\MonetizationPackage $monetization_package
     */
    public function setMonetizationPackage(MonetizationPackage $monetization_package) {
      $this->monetization_package = $monetization_package;
    }

    /**
     * Set Rate Plan currency
     * @param \Apigee\Mint\DataStructures\SupportedCurrency $currency
     */
    public function setCurrency(DataStructures\SupportedCurrency $currency) {
      $this->currency = $currency;
    }

    /**
     * Set Parent Rate Plan
     * @param \Apigee\Mint\RatePlan $parent_rate_plan
     */
    public function setParentRatePlan(RatePlan $parent_rate_plan) {
      $parent_rate_plan->setChildRatePlan($this);
      $this->parent_rate_plan = $parent_rate_plan;
    }

    public function setPaymentDueDays($payment_due_days) {
      $this->payment_due_days = $payment_due_days;
    }

    /**
     * Only for internal logic
     * @param RatePlan $rate_plan
     */
    public function setChildRatePlan(RatePlan $rate_plan) {
      $this->child_rate_plan = $rate_plan;
    }

    /**
     * Set com.apigee.mint.model.Developer
     * @param \Apigee\Mint\Developer $developer
     */
    public function setDeveloper(Developer $developer) {
      $this->developer = $developer;
    }

    /**
     * Set com.apigee.mint.model.DeveloperCategory
     * @param \Apigee\Mint\DeveloperCategory $developer_category
     */
    public function setDeveloperCategory(DeveloperCategory $developer_category) {
      $this->developer_category = $developer_category;
    }

    /**
     * Add a of DeveloperRatePlan
     * @param \Apigee\Mint\DeveloperRatePlan $developer_rate_plan
     */
    public function addDeveloperRatePlan(DeveloperRatePlan $developer_rate_plan) {
      $this->developers[] = $developer_rate_plan;
    }

    /**
     * Remove all DeveloperRatePlans from this RatePlan
     */
    public function clearDeveloperRatePlan() {
      $this->developers = array();
    }

    /**
     * Set com.apigee.mint.model.ApplicationCategory
     * @param \Apigee\Mint\ApplicationCategory $application_category
     */
    public function setApplicationCategory($application_category) {
      $this->application_category = $application_category;
    }

    /**
     * Set Exchange Organization
     * @param \Apigee\Mint\Organization $exchange_organization
     */
    public function setExchangeOrganization($exchange_organization) {
      $this->exchange_organization = $exchange_organization;
    }

    /**
     * Set Rate plan type.
     * @param string $type Possible values: STANDARD|DEVELOPER_CATEGORY|DEVELOPER|APPLICATION_CATEGORY|EXCHANGE_ORGANIZATION
     * @throws \Apigee\Exceptions\ParameterException
     */
    public function setType($type) {
      $type = strtoupper($type);
      if (!in_array($type, array(
        'STANDARD',
        'DEVELOPER_CATEGORY',
        'DEVELOPER',
        'APPLICATION_CATEGORY',
        'EXCHANGE_ORGANIZATION'
      ))
      ) {
        throw new ParameterException('Invalid type of RatePlan: ' . $type);
      }
      $this->type = $type;
    }

    /**
     * Set Name
     * @param string
     */
    public function setName($name) {
      $this->name = $name;
    }

    /**
     * Set Display Name
     * @param string $display_name
     */
    public function setDisplayName($display_name) {
      $this->display_name = $display_name;
    }

    /**
     * Set Description
     * @param string $description
     */
    public function setDescription($description) {
      $this->description = $description;
    }

    /**
     * Set One time set up fee
     * @param double $set_up_fee
     */
    public function setSetUpFee($set_up_fee) {
      $this->set_up_fee = $set_up_fee;
    }

    /**
     * Set Recurring time set up fee
     * @param double $recurring_fee
     */
    public function setRecurringFee($recurring_fee) {
      $this->recurring_fee = $recurring_fee;
    }

    /**
     * Set Frequency Duration
     * @param int $frequency_duration
     */
    public function setFrequencyDuration($frequency_duration) {
      $this->frequency_duration = $frequency_duration;
    }

    /**
     * Set Frequency Duration Type.
     * @param string $frequency_duration_type Possible values: DAY|WEEK|MONTH|QUARTER|YEAR
     * @throws \Apigee\Exceptions\ParameterException
     */
    public function setFrequencyDurationType($frequency_duration_type) {
      $frequency_duration_type = strtoupper($frequency_duration_type);
      if (!in_array($frequency_duration_type, array('DAY', 'WEEK', 'MONTH', 'QUARTER', 'YEAR'))) {
        throw new ParameterException('Invalid frequency duration type: ' . $frequency_duration_type);
      }
      $this->frequency_duration_type = $frequency_duration_type;
    }

    /**
     * Set Recurring Type Used to define if scheduler needs to be run based on Calendar or Custom (plan start date)
     * @param string $recurring_type. Possible values: CALENDAR|CUSTOM
     * @throws \Apigee\Exceptions\ParameterException
     */
    public function setRecurringType($recurring_type) {
      $recurring_type = strtoupper($recurring_type);
      if (!in_array($recurring_type, array('CALENDAR', 'CUSTOM'))) {
        throw new ParameterException('Invalid recurring type: ' . $recurring_type);
      }
      $this->recurring_type = $recurring_type;
    }

    /**
     * Set Recurring start unit this is only used if the recurringType is CALENDAR
     * @param int $recurring_start_unit
     */
    public function setRecurringStartUnit($recurring_start_unit) {
      $this->recurring_start_unit = $recurring_start_unit;
    }

    /**
     * Set if this package is to be prorated
     * @param boolean $prorate
     */
    public function setProrate($prorate) {
      $this->prorate = $prorate;
    }

    /**
     * Set Early termination fee
     * @param double $early_termination_fee
     */
    public function setEarlyTerminationFee($early_termination_fee) {
      $this->early_termination_fee = $early_termination_fee;
    }

    /**
     * Set Effective Start date
     * @param string $start_date
     */
    public function setStartDate($start_date) {
      $this->start_date = $start_date;
    }

    /**
     * Set Effective End date
     * @param string $end_date
     */
    public function setEndDate($end_date) {
      $this->end_date = $end_date;
    }

    /**
     * Set Freemium duration
     * @param int $freemium_duration
     */
    public function setFreemiumDuration($freemium_duration) {
      $this->freemium_duration = $freemium_duration;
    }

    /**
     * Set Freemium number of units
     * @param int $freemium_unit
     */
    public function setFreemiumUnit($freemium_unit) {
      $this->freemium_unit = $freemium_unit;
    }

    /**
     * Set Freemium Duration Type
     * @param string $freemium_duration_type
     * @throws \Apigee\Exceptions\ParameterException
     */
    public function setFreemiumDurationType($freemium_duration_type) {
      $freemium_duration_type = strtoupper($freemium_duration_type);
      if (!in_array($freemium_duration_type, array('DAY', 'WEEK', 'MONTH', 'QUARTER', 'YEAR'))) {
        throw new ParameterException('Invalid freemium duration type: ' . $freemium_duration_type);
      }
      $this->freemium_duration_type = $freemium_duration_type;
    }

    /**
     * Set published
     * @param boolean $published
     */
    public function setPublished($published) {
      $this->published = $published;
    }

    /**
     * Set Contract duration
     * @param int $contract_duration
     */
    public function setContractDuration($contract_duration) {
      $this->contract_duration = $contract_duration;
    }

    /**
     * Set Contract Duration Type .
     * @param string $contract_duration_type
     * @throws \Apigee\Exceptions\ParameterException
     */
    public function setContractDurationType($contract_duration_type) {
      $contract_duration_type = strtoupper($contract_duration_type);
      if (!in_array($contract_duration_type, array('DAY', 'WEEK', 'MONTH', 'QUARTER', 'YEAR'))) {
        throw new ParameterException('Invalid contract duration type: ' . $contract_duration_type);
      }
      $this->contract_duration_type = $contract_duration_type;
    }

    /**
     * Keep developer original start date (used for rate plan revisions)
     * @param boolean $keep_original_start_date
     */
    public function setKeepOriginalStartDate($keep_original_start_date) {
      $this->keep_original_start_date = $keep_original_start_date;
    }

    /**
     * Add Rate plan details
     * @param \Apigee\Mint\DataStructures\RatePlanRate $rate_plan_detail
     */
    public function addRatePlanDetails(DataStructures\RatePlanRate $rate_plan_detail) {
      $this->rate_plan_details[] = $rate_plan_detail;
    }

    /**
     * Remove all RatePlanDetail from this RatePlan
     */
    public function clearRatePlanDetails() {
      $this->rate_plan_details = array();
    }

    public function getId() {
      return $this->id;
    }

    // Used in data load invoked by $this->loadFromRawData()
    private function setId($id) {
      $this->id = $id;
    }

    public function isGroupPlan() {
      $is_group_plan = TRUE;
      foreach ($this->rate_plan_details as $ratePlanDetails) {
        if ($ratePlanDetails->getOrganization()->getParent() == NULL) {
          $is_group_plan = FALSE;
          break;
        }
        else if ($ratePlanDetails->getOrganization()->getParent()->getId() != $this->organization->getId()) {
          $is_group_plan = FALSE;
          break;
        }
      }
      return $is_group_plan;
    }

    public function load($id = NULL) {
      if (!isset($id)) {
        $id = $this->{$this->id_field};
      }
      if (!isset($id)) {
        throw new ParameterException('No object identifier was specified.');
      }
      $cache_manager = CacheFactory::getCacheManager(NULL);
      $data = $cache_manager->get('rate_plan:' . $id, NULL);
      if (!isset($data)) {
        $url = $this->base_url . '/' . rawurlencode($id);
        $this->client->get($url);
        $data = $this->getResponse();
        $cache_manager->set('rate_plan:' . $id, $data);
      }
      $this->initValues();
      $this->loadFromRawData($data);
    }
  }
}
