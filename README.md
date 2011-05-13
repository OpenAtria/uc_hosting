Aegir Ubercart Integration
==========================

__This is a beta release!__

This module provides several features to ubercart products on an Aegir platform.

Requirements
------------

* Hostmaster 6.x-1.0 or higher
* Ubercart 6.x-2.4 or higher
* Patience

Installation
------------

1. Download the module and its dependencies into sites/all/modules on your hostmaster platform (`drush @hostmaster dl uc_hosting ubercart` should work)
2. Activate the clients feature for hostmaster (`admin/hosting/features`)
3. Enable the module uc_hosting_products. This should enable all dependencies (`drush @hostmaster en uc_hosting_products`)

Creating your first site product
--------------------------------

1. Create a product. Make sure it is not shippable. (`node/add/product`)
2. Edit the product. Go to the features tab.
3. Choose the "Create a site and adjust quotas accordingly" feature. It should use the sku of your product automatically.
4. Now all you need is a drupal platform that any user can create platforms on and you are ready to go. (`node/add/platform`)

Creating more complex offerings using product kits
--------------------------------------------------

The method we recommend for creating complex offerings, including multiple site quotas and access to restricted platforms, is via Ubercart product kits.

1. Enable uc_product_kit.
2. Create one or more uc_hosting products.
3. Create a product kit with these products in them in the appropriate quantities.

Support
-------

You can report bugs, request support and propose patches [via our issue queue on Drupal.org](http://drupal.org/project/issues/uc_hosting).

Development
-----------

Check out the [roadmap](http://community.aegirproject.org/node/108).

Other Resources
---------------

* [Aegir community site](http://community.aegirproject.org)
* [Ubercart Documentation](http://www.ubercart.org/docs)
* #aegir IRC channel on [freenode](http://freenode.net/)
