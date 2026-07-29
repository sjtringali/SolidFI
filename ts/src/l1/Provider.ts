// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';

export abstract class Provider<T, U, P extends Parameters = Parameters>
    implements Converter<T, U, P> {
    abstract resolve(value: T, params: P): U;
}
