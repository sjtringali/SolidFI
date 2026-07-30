// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter as IConverter } from './Converter';
import { Transform as ITransform } from './Transform';
import { Parameters } from './Parameters';

export function Deferred<T, U, P extends Parameters>(ctor: new () => IConverter<T, U, P>): IConverter<T, U, P>;
export function Deferred<T, P extends Parameters>(ctor: new () => ITransform<T, P>): ITransform<T, P>;
export function Deferred(ctor: any): IConverter<any, any> | ITransform<any> {
    if ('resolve' in ctor.prototype) {
        return new Deferred.Converter(ctor);
    }
    if ('apply' in ctor.prototype) {
        return new Deferred.Transform(ctor);
    }
    throw new Error(`${ctor.name} is neither a Converter nor a Transform`);
}

export namespace Deferred {
    export class Converter<T, U, P extends Parameters = Parameters> implements IConverter<T, U, P> {
        private instance: IConverter<T, U, P> | undefined;

        constructor(private readonly factory: new () => IConverter<T, U, P>) {}

        acquire(value: T, params: P): U {
            if (!this.instance) {
                this.instance = new this.factory();
            }
            return this.instance.resolve(value, params);
        }

        resolve(value: T, params: P): U {
            return this.acquire(value, params);
        }
    }

    export class Transform<T, P extends Parameters = Parameters> implements ITransform<T, P> {
        private instance: ITransform<T, P> | undefined;

        constructor(private readonly factory: new () => ITransform<T, P>) {}

        acquire(value: T, params: P): T {
            if (!this.instance) {
                this.instance = new this.factory();
            }
            return this.instance.apply(value, params);
        }

        apply(value: T, params: P): T {
            return this.acquire(value, params);
        }
    }
}
