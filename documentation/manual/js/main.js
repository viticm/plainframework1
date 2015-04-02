/**
 * Apply google prettyprint to all <pre> elements within the
 * documentation.
 */
jQuery(document).ready(function($)
{
	$('.content pre, table code').addClass('prettyprint');
	prettyPrint();

	var amount = null;

	$('#donateButton').click(function(e){
		var token = function(res){
			var $input = $('<input type=hidden name=stripeToken />').val(res.id),
				$amountInput = $('<input type=hidden name=amount />').val(amount);
			$('form').append($input).append($amountInput).submit();
		};

		e.preventDefault();

		amount = parseInt(prompt("How much would you like to donate in US dollars?", '10').trim());

		if (isNaN(amount) || amount < 1)
		{
			alert("Sorry, you must enter a numerical value of $1 or more!");
		}
		else
		{
			StripeCheckout.open({
				key:         'pk_MsRVTNtBo9ojXn7PyUQmjrmlvnTIe',
				amount:      amount * 100,
				image: 		 'http://frozennode.com/images/fn-icon-128.png',
				currency:    'usd',
				name:        'FrozenNode',
				description: 'Donate to Administrator',
				panelLabel:  'Donate',
				token:       token
			});
		}
	});
});
