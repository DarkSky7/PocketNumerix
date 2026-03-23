<?php /*DO NOT MODIFY THIS LINE!*/ if (isset($SC['cart_message'])) { ?>
<table width="100%">
	<tr>
		<td class="content">
			<?php
				print $SC['cart_message']
			?>
		</td>
	</tr>
</table>
<?php /*DO NOT MODIFY THIS LINE!*/ } ?>

<?php /*DO NOT MODIFY THIS LINE!*/ if (isset($SC['templates']['best_selller_single'])) { ?>
<table width="100%">
	<tr>
		<td class="content">
			<?php
				// Best Seller (Single Item) section
				// code below will display your top selling item
 				show_best_sellers(1, $SC['templates']['best_seller_single']);
			?>
		</td>
	</tr>
</table>
<?php /*DO NOT MODIFY THIS LINE!*/ } ?>

<?php
	// New Product Preview section - code below will display a preview of new products in your catalog
	new_product_preview();
?>

<table width="100%">
	<tr>
		<td class="content">
			<?php
				// Category Detail section
				// code below will show all categories in cart for browsing
 				show_categories_detail();
			?>
		</td>
	</tr>
</table>

