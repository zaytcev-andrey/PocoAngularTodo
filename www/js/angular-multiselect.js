angular.module('ui.multopleselector', []).
directive('tdMultipleselector',['$parse', '$timeout', function ($parse, $timeout) { 
	return {
	    link: function (scope, element, attrs, controller) {
	        return $timeout( function () {	            
	                console.log("tdMultipleselector $timeout");
	                console.log("current_data is : " + scope.protectionclasses);

	                var ngModel = $parse(attrs.ngModel);
	                $(function () {

	                    element.multipleSelector().click(function () {
	                        var $this = $(this);
	                        var modelPath = $this.attr('ng-model');
	                        var selectedData = $this.multipleSelector('getdata');
	                        scope.$apply(function (scope) {
	                            // Change binded variable
	                            ngModel.assign(scope, selectedData);
	                        });
	                        console.log("#angular binary clicked");
	                        console.log("selected items : " + selectedData);
	                    });
	                });
	        }, 1000, false);
		}
	};
}]);