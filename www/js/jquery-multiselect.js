(function( $ ) {
	
	var selector_uid = 0;
	
	function ISelectControl( $wrapperfilter ) {                          	
    	this._$ul = $($wrapperfilter).children("ul#filter-bar");
        this._$li = this._$ul.children("li.filter-option");
        $($wrapperfilter).width( this._$li.length * this._$li.width() );
        this._data_target = [];
        var dt_str = this._$ul.attr( "data-target" ) ? this._$ul.attr( "data-target" ) : "";        
        this._data_target = dt_str.split(',');
        
        this._get_li_by_target = function( data_target ) {
        	return this._$ul.children("li.filter-option[data-target='" + data_target + "']");        	        
        }
        
        this._id = "multiselect_uid" + ++selector_uid;
        $($wrapperfilter).attr( "multiselect_uid", this._id );
    }
    
    ISelectControl.prototype.Select = function($element) {
         throw new Error("ISelectControl.Select not implemented");
    }
    
    ISelectControl.prototype.GetSelected = function($element) {
        return this._data_target;
    }
    
    ISelectControl.prototype.GetId = function($element) {
         return this._id;
    }
    
    function MultipleSelectControl($wrapperfilter){
        
    	ISelectControl.apply(this, $wrapperfilter);
    	
    	console.log( "initial data-target : " + this._data_target );
		
		for ( var id in this._data_target ) {
			var $element = this._get_li_by_target( this._data_target[ id ] );
			$element.addClass("active");
		}
		
		var control_ = this;
        this._$li.click(function(){
            control_.Select($(this));
        });
    }
    
    MultipleSelectControl.prototype = Object.create(ISelectControl.prototype);
    MultipleSelectControl.prototype.constructor = MultipleSelectControl;
    
    MultipleSelectControl.prototype.Select = function($element) {
    	if ( $element.hasClass("active") ) {
			$element.removeClass("active");
			var index = this._data_target.indexOf( $element.attr("data-target") );
			this._data_target.splice( index, 1 );
		} else {
			$element.addClass("active");
			this._data_target.push( $element.attr("data-target") );
		}
        
        this._$ul.attr( "data-target", this._data_target );
    }
	
    function ToggleSelectControl($wrapperfilter){
        
    	ISelectControl.apply(this, $wrapperfilter);
		
		if ( this._data_target.length > 1 )
			throw new Error("ToggleSelectControl can not have more than one element in data-target attribute");
		
		this._get_li_by_target( this._data_target[ 0 ] ).addClass("active");

		var control_ = this;
        this._$li.click(function(){
            control_.Select($(this));
        });
    }
	
	ToggleSelectControl.prototype = Object.create(ISelectControl.prototype);
    ToggleSelectControl.prototype.constructor = ToggleSelectControl;
    
    ToggleSelectControl.prototype.Select = function($element) {
    	this._$li.removeClass("active");
    	$element.addClass("active");     
    	this._data_target = $element.attr("data-target");
        this._$ul.attr( "data-target", this._data_target );
    }
	
	function CreateSelector( element ) {
		var $element = $(element);
		var select_method = $element.attr("select-method");
		switch (select_method) {
			case "multiple":
				return new MultipleSelectControl( $element );
			case "toggle":
				return new ToggleSelectControl( $element );
			default:
				throw new Error("Selector " + select_method + " not implemented");
		}
	}
	
	var selectors = {};
	
	var methods = {
		init : function() {
			item = CreateSelector( this.first() );
			selectors[item.GetId()] = item;
			return this;
		},
		getdata : function() {
			var uid = $(this).attr( "multiselect_uid" );			
			console.log('in get function : jquery is ' + uid);
			return selectors[uid].GetSelected();
		}
	};
	
	$.fn.multipleSelector = function( method ){
		
		if ( methods[ method ] ) {
			return methods[method].apply( this, Array.prototype.slice.call( arguments, 1 ));
		} else if ( typeof method === 'object' || !method ) {
		      return methods.init.apply( this, arguments );
		} else {
		      $.error( 'There is no method "' +  method + '" for jQuery.multipleSelector' );
		}  
	}
})(jQuery);
